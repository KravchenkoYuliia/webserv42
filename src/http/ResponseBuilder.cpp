/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 18:00:12 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> //opendir
#include <sys/wait.h>
#include <ctime>
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"
#include "utils/Utils.hpp"

//std::cout << "\e[1;92m" << "\033[0m" << std::endl;

ResponseBuilder::ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data, size_t request_error ) {

	initialize_values( request, config_data );

	if ( request_error != 1 ) {
		setErrorState( request_error );
	}
	else {
		buildResponse( request );
	}

	if ( error_ == true ) {
		buildErrorResponse();
	}

    setCookie();
	setContentLength();
	setLastLineOfHeader();
}

ResponseBuilder::ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data, const std::string& raw_cgi_output )
{
    initialize_values( request, config_data );

    buildCgiResponse( raw_cgi_output );

    if ( error_ == true ) {
        buildErrorResponse();
    }

    setContentLength();
    setLastLineOfHeader();
}

ResponseBuilder::~ResponseBuilder() {}

void	ResponseBuilder::buildResponse( const HttpRequest& request ) {

	if ( returnInRequest() == true )
		buildReturn( config_data_.getReturn() );
	else
		buildResponseAccordingToMethod( request );
}


void ResponseBuilder::buildCgiResponse(const std::string& raw_cgi_output)
{
    const std::string   separator = Http::Formatting::CRLF;
    size_t              sep_pos = raw_cgi_output.find(separator);
    if (sep_pos == std::string::npos)
    {
        std::cerr << "[ResponseBuilder::buildCgiResponse] no CRLF separator found, will return 502 "<< std::endl;
        setErrorState(502);
        return;
    }

    std::string cgi_headers = raw_cgi_output.substr(0, sep_pos);
    std::string body        = raw_cgi_output.substr(sep_pos + separator.size());

    code_ = 200;
    std::string content_type = Http::ContentType::TEXT_HTML;
    size_t      start_pos = 0;
    while (start_pos < cgi_headers.size())
    {
        size_t end_pos = cgi_headers.find(Http::Formatting::CRLF, start_pos);
        if (end_pos == std::string::npos)
            end_pos = cgi_headers.size();
        std::string line = Utils::toLower(cgi_headers.substr(start_pos, end_pos - start_pos));
        if (!line.empty())
        {
            static const std::string status = "status: ";
            static const std::string content_type_header = "content-type: ";
            if (line.find(status) == 0)
            {
                std::istringstream ss(line.substr(status.size()));
                ss >> code_;
            }
            else if (line.find(content_type_header) == 0)
                content_type = line.substr(content_type_header.size());
            else
                header_ << line << Http::Formatting::CRLF;
        }
        if (end_pos == cgi_headers.size())
            break;
        start_pos = end_pos + Http::Formatting::CRLF_SIZE;
    }
    setStatusCode();
    setServerAndDate();
    header_ << Http::Headers::CONTENT_TYPE << ": " << content_type << Http::Formatting::CRLF;
    response_.setBody(body);
}

bool	ResponseBuilder::returnInRequest() {

	if ( config_data_.getReturn().empty() )
		return false;

	return true;
}

void	ResponseBuilder::setCookie() {

	if ( query_ != "" )
		header_ << "Set-cookie: " << query_ << Http::Formatting::CRLF;
	else if ( !cookie_.empty() ) {
		header_ << "Set-cookie: " << cookie_.begin()->second << Http::Formatting::CRLF;
	}
}

void	ResponseBuilder::initialize_values( const HttpRequest& request, const MergedConfig& config_data ) {

	error_ = false;
	file_or_dir_ = NOT_SPECIFIED;
	code_ = NOT_SPECIFIED;
	code_meaning_ = "";
	header_.str("");
	config_data_ = config_data;
	cutQueryFromUri( request.getUri() );
	if ( !request.getCookie().empty() )
		cookie_ = request.getCookie();
	headers_in_request = request.getHeaders();
}

const HttpResponse&	ResponseBuilder::getResponse() {

	return response_;
}

void	ResponseBuilder::setStatusCode() {

	code_meaning_ = getCodeMeaning();
	if ( code_meaning_ == "This code is not implemented" ) {
		code_ = 500;
		code_meaning_ = getCodeMeaning();
	}

	header_ << Http::Protocol::HTTP_VERSION_1_0 << " " << code_ << " " << code_meaning_ << Http::Formatting::CRLF;
}

void	ResponseBuilder::setContentLength() {

	header_ << Http::Headers::CONTENT_LENGTH << ": " << response_.getBody().length() << Http::Formatting::CRLF;
}

void	ResponseBuilder::setLastLineOfHeader() {

	header_ << "Connection: keep-alive" << Http::Formatting::HEADER_END;
	response_.setHeader( header_ );
}

void	ResponseBuilder::buildReturn( const std::map<int, std::string>& return_data ) {

	code_ = return_data.begin()->first;
	std::string	what_is_return = return_data.begin()->second;

	setStatusCode();
	setServerAndDate();
	if ( code_ >= 301 && code_ <= 308 ) {
		buildRedirectionReturn( what_is_return );
	}
	else {
		buildBasicReturn( what_is_return );
	}
}

void	ResponseBuilder::buildRedirectionReturn( const std::string& what_is_return ) {

	header_ << "Location: " << what_is_return << Http::Formatting::CRLF;
	header_ << Http::Headers::CONTENT_TYPE << ": " << "text/html" << Http::Formatting::CRLF;
	response_.setBody( generateDefaultPage( "" ) );
}

void	ResponseBuilder::buildBasicReturn( const std::string& what_is_return ) {

	header_ << Http::Headers::CONTENT_TYPE << ": text/plain" << Http::Formatting::CRLF;
	response_.setBody( what_is_return );
}

void	ResponseBuilder::buildResponseAccordingToMethod( const HttpRequest& request ) {

	if ( checkMethodInRequest( request.getMethod(), config_data_.getMethods() ) == ERROR ) {

		ResponseBuilder::setErrorState( 405 );
		return ;
	}

	if ( request.getMethod() == HttpRequest::GET ) {

		buildResponseGET();
	}
	else if ( request.getMethod() == HttpRequest::POST ) {

		if ( checkBodySize( request.getContentLength(), config_data_.getMaxBodySize() ) == ERROR ) {
			setErrorState( 413 );
			return ;
		}

		buildResponsePOST( request );
	}
	else if ( request.getMethod() == HttpRequest::DELETE ) {

		buildResponseDELETE( request );
	}
}

void	ResponseBuilder::buildResponseGET() {

	code_ = 200;
	setStatusCode();
	setServerAndDate();
	std::string	path_without_prefix = cutPrefixFromUri( uri_ );
	handleUri( path_without_prefix );
}

void	ResponseBuilder::buildResponsePOST( const HttpRequest& request ) {

	if ( request.getHeaderValue( "Content-Type" ) == Http::ContentType::TEXT_PLAIN ) {

		int return_value = handlePlainText( request.getBody() );
		if ( return_value == ERROR ) {
			setErrorState( 500 );
			return ;
		}
	}
	else if ( config_data_.getUploadAllowed() == true ) {
		handleUpload( request );
	}
	else
		setErrorState( 403 );
}

int	ResponseBuilder::handlePlainText( const std::string& body ) {

	std::ofstream file;
	const std::string path = buildPathFromRootAndResource( config_data_.getRoot(), "text_uploaded_to_server.txt");
	file.open( path.c_str() );
	if ( !file.is_open() )
		return ERROR;

	file << body;
	buildSuccessUploadResponse( "text_uploaded_to_server.txt" );
	return SUCCESS;
}

void	ResponseBuilder::buildResponseDELETE( const HttpRequest& request ) {

	if ( deleteFile( request.getUri() ) == ERROR )
		return ;

	code_ = 200;
	setStatusCode();
	setServerAndDate();
	header_ << Http::Headers::CONTENT_TYPE << ": " << "text/html" << Http::Formatting::CRLF;
	const std::string&	body = generateDefaultPage( "Successfully deleted " + request.getUri() );
	response_.setBody( body );
}

int	ResponseBuilder::deleteFile( const std::string& uri ) {

	std::string		uri_without_prefix = cutPrefixFromUri( uri );
	const std::string	path = buildPathFromRootAndResource( config_data_.getRoot(), uri_without_prefix );

	if ( checkIfPathExists( path ) == ERROR )
		return ERROR;

	if ( file_or_dir_ != IS_FILE ) {
	        setErrorState( 403 );
		return ERROR;
    	}

	int return_value = std::remove( path.c_str() );
	if ( return_value != SUCCESS ) {
		if ( errno == ENOENT )
			setErrorState( 404 );
		else if ( errno == EACCES || errno == EPERM || errno == EISDIR )
			setErrorState( 403 );
		else
			setErrorState( 500 );
		return ERROR;
	}

	return SUCCESS;
}

void	ResponseBuilder::buildErrorResponse() {

	setStatusCode();
	setHeaderLineFor405Error();
	setServerAndDate();

	bool					error_page_from_config = false;
	const std::map<int, std::string>&	config_errors = config_data_.getErrorPage();
	std::string				root = config_data_.getRoot();
	for ( std::map<int, std::string>::const_iterator it = config_errors.begin(); it != config_errors.end(); it++ ) {
		if ( it->first == code_ ) {
			const std::string path = buildPathFromRootAndResource( root, it->second );
			setErrorPageHtml( path );
			if ( response_.getBody() != "" )
				error_page_from_config = true;
		}
	}
	if ( error_page_from_config == false )
		setErrorPageHtml( "" );
}

void	ResponseBuilder::handleUri( const std::string uri ) {

	const std::string path = buildPathFromRootAndResource( config_data_.getRoot(), uri );
	if ( checkIfPathExists( path ) == ERROR )
		return ;

	if ( file_or_dir_ == IS_FILE ) {
		if ( handleFile( path ) == ERROR )
			return ;
	}
	else if ( file_or_dir_ == IS_DIR ) {
		handleDirectory( path );
	}
}

int	ResponseBuilder::handleFile( const std::string& path ) {

	if ( checkFilePermissions( path.c_str() ) == ERROR )
		return ERROR;
	setContentType( path );

	std::string	body;
	body = readContentFromFile( path );

	std::string	username_cookie = getUsernameCookie();
	if ( username_cookie != "" )
		replaceUsernameByCookie( body, username_cookie );

	if ( error_ == true ) {
		setErrorState( 500 );
		return ERROR;
	}

	response_.setBody( body );
	return SUCCESS;
}

const std::string	ResponseBuilder::getUsernameCookie() {

	if ( query_ != "" && query_.length() > 8 && query_.substr( 0, 9 ) == "username=" ) {
		return query_.substr( 9 );
	}
	else if ( !cookie_.empty() ) {

		std::string all_cookies = cookie_["cookie"];
		size_t pos_of_username = all_cookies.find("username=");
		if ( pos_of_username == all_cookies.npos )
			return "";
		char	username[1000];
		int i = 0;
		pos_of_username += 9;
		while ( pos_of_username < all_cookies.length() && std::isalnum( all_cookies[pos_of_username] ) && i < 1000 ) {
			username[i] = all_cookies[pos_of_username];
			i++;
			pos_of_username++;
			username[i] = '\0';
		}

		std::string username_str = username;
		return username_str;
	}
	return "";
}

void	ResponseBuilder::replaceUsernameByCookie( std::string& body, const std::string& username_cookie ) {

	size_t	pos_of_username = body.find( "user" );
	if ( pos_of_username == body.npos )
		return ;
	body.replace( pos_of_username, 4, username_cookie );
}

void	ResponseBuilder::handleDirectory( const std::string& path_without_file ) {

	const std::vector<std::string>&	indices = config_data_.getIndex();
	for ( std::vector<std::string>::size_type i = 0; i < indices.size(); i++ ) {

		std::string	path = buildPathFromRootAndResource( path_without_file, indices[i] );
		if ( access( path.c_str(), F_OK ) == 0 ) {
			handleFile( path );
			return ;
		}
	}
	handleAutoindex( path_without_file );
}

void	ResponseBuilder::handleAutoindex( const std::string& path ) {

	if ( config_data_.getAutoindex() == AUTOINDEX_OFF ) {
		setErrorState( 403 );
		return;
	}

	DIR* dir_ptr = openDirectory( path );
	if ( dir_ptr == NULL )
		return ;

	std::vector<std::string>	files_from_dir;
	readDirectory( dir_ptr, files_from_dir );

	buildListing( files_from_dir );
}

void	ResponseBuilder::handleUpload( const HttpRequest& request ) {

	const std::string&	request_body = request.getBody();
	const std::string	filename = getFileNametoUpload( request_body );
	if ( filename == "") {

		std::cerr << "Upload: No file name to upload" << std::endl;
		setErrorState( 400 );
		return ;
	}
	const std::string	file_content = getFileContent( request_body );
	if ( file_content == "" && code_ == 400 ) {
		setErrorState( 400 );
		return ;
	}
	int return_status = createUploadedFile( filename, file_content );
	if ( return_status == ERROR ) {
		setErrorState( 500 );
		return ;
	}
	buildSuccessUploadResponse( filename );
}

void	ResponseBuilder::buildSuccessUploadResponse( const std::string& filename ) {

	code_ = 200;
	setStatusCode();
	setServerAndDate();
	header_ << Http::Headers::CONTENT_TYPE << ": " << "text/html" << Http::Formatting::CRLF;

	const std::string	body = generateDefaultPage( "Successfully uploaded " + filename );
	response_.setBody( body );
}

const std::string	ResponseBuilder::getFileNametoUpload( const std::string& request_body ) {

	size_t	position_of_start  = request_body.find( "filename=\"" ) + 10;
	if ( position_of_start == request_body.npos)
		return "";

	size_t	position_of_end    = request_body.find( Http::Formatting::CRLF, position_of_start );
	if ( position_of_end == request_body.npos)
		return "";

	size_t	length_of_filename = position_of_end - position_of_start - 1;
	return request_body.substr( position_of_start, length_of_filename );
}

const std::string	ResponseBuilder::getFileContent( const std::string& request_body ) {

	size_t	position_of_start = request_body.find( Http::Formatting::HEADER_END ) + 4;
	if ( position_of_start == request_body.npos) {
		code_ = 400;
		return "";
	}

	const std::string boundary = "--" + getBoundaryFromHeaders() + "--";
	if ( boundary == "--" && code_ == 400 )
		return "";
	size_t	position_of_end = request_body.find( boundary ) - 2;
	if ( position_of_end == request_body.npos ) {
		code_ = 400;
		return "";
	}

	size_t	length_of_filename = position_of_end - position_of_start;
	return request_body.substr( position_of_start, length_of_filename );
}

std::string	ResponseBuilder::getBoundaryFromHeaders() {

	bool	is_content_type = false;
	for ( std::map<std::string, std::string>::const_iterator it = headers_in_request.begin(); it != headers_in_request.end(); it++ ) {
		if ( it->first == "content-type" ) {
			is_content_type = true;
			size_t pos_of_boundary = it->second.find( "boundary=" );
			if ( pos_of_boundary == it->second.npos ) {
				std::cerr << "Error: Boundary is missing" << std::endl;
				code_ = 400;
				return "";
			}
			return it->second.substr( pos_of_boundary + 9 );
		}
	}
	if ( is_content_type == false ) {
		std::cerr << "Error: Content-type is missing" << std::endl;
		code_ = 400;
		return "";
	}

	return "";
}

int	ResponseBuilder::createUploadedFile( const std::string& filename, const std::string& file_content ) {

	upload_path_ = buildPathFromRootAndResource( config_data_.getUploadLocation(), filename );

	std::ofstream	file;
	file.open( upload_path_.c_str() );
	if ( !file.is_open() )
		return ERROR;

	file << file_content;
	file.close();
	return SUCCESS;
}

void	ResponseBuilder::setContentType( const std::string& path ) {

	std::string		content_type;
	const std::string	extension = getExtension( path );

	if ( extension == "No extension" )
		content_type = "application/octet-stream";
	else if ( extension == "jpg" || extension == "jpeg" || extension == "png" )
		content_type = "image/" + extension;
	else
		content_type = "text/" + extension;

	header_ << Http::Headers::CONTENT_TYPE << ": " << content_type << Http::Formatting::CRLF;
}


void	ResponseBuilder::setServerAndDate() {

	header_ << "Server: 📡✅ Webserv 🐝" << Http::Formatting::CRLF;

	time_t		t;
	struct tm*	t_struct;
	std::time( &t );
	t_struct = std::localtime( &t );
	std::string	date = asctime( t_struct );

	header_ << "Date: " << date.substr( 0, date.length()-1 ) << Http::Formatting::CRLF;
}

void	ResponseBuilder::buildListing( std::vector<std::string>& files_from_dir ) {

	header_ << Http::Headers::CONTENT_TYPE << ": " << "text/html" << Http::Formatting::CRLF;

	std::string		path;
	std::stringstream	body;
	body << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>Index of " << uri_ << "</title></head>\n"
		<< "<h2> &#128221; Index of &#128193;" << uri_ << "</h2>\n"
		<< "<hr><body>\n";
	for ( std::vector<std::string>::size_type i = 0; i < files_from_dir.size(); i++ ) {
		path = buildPathFromRootAndResource( uri_, files_from_dir[i] );
		body << "<a href=\"" << path << "\">" << files_from_dir[i] << "</a><br>\n";
	}
	body << "</body>\n</html>\n";
	response_.setBody( body.str() );
}

int	ResponseBuilder::checkIfPathExists( const std::string& path ) {

	struct stat s;

	int result = stat( path.c_str(), &s );
	if ( result == -1 ) {

		if ( errno == EACCES )
			setErrorState( 403 );
		else
			setErrorState( 404 );
		return ERROR;
	}

	if ( S_ISREG( s.st_mode ) != 0 ) {
		file_or_dir_ = IS_FILE;
	}
	else if ( S_ISDIR( s.st_mode ) != 0 ) {
		file_or_dir_ = IS_DIR;
	}
	else {
		setErrorState( 403 );
		return ERROR;
	}

	return SUCCESS;
}

int	ResponseBuilder::checkBodySize( size_t current_body_size, size_t max_body_size ) {

	if ( current_body_size > max_body_size ) {

		return ERROR;
	}
	return SUCCESS;
}

void	ResponseBuilder::setErrorPageHtml( const std::string page_from_config ) {

	header_ << Http::Headers::CONTENT_TYPE << ": text/html" << Http::Formatting::CRLF;

	std::string	body;
	if ( page_from_config != "")
		body = readContentFromFile( page_from_config );
	if ( body == "" )
		body = generateDefaultPage( "" );
	response_.setBody( body );
}

const std::string	ResponseBuilder::generateDefaultPage( const std::string& optionnal_message ) {

	std::stringstream	default_page;

	default_page << "<!DOCTYPE html>\n"
			<< "<html>\n"
			<< "<head><title>"
			<< code_ << " " << code_meaning_
			<<"</title></head><br>\n"
			<< "<body>\n"
			<< "<center><h1>"
			<< code_ << " " << code_meaning_
			<< "<br><br>"
			<< optionnal_message
			<<"</h1></center>\n"
			<< "<hr><center> &#128225;&#9989; Webserv &#128029;</center>\n"
			<< "</body>\n"
			<< "</html>";

	return default_page.str();
}

int	ResponseBuilder::checkMethodInRequest( HttpRequest::Method current_method, const std::vector<std::string>& allowed_methods ) {

	for ( std::vector<std::string>::size_type i = 0; i < allowed_methods.size(); i++ ) {
		if ( allowed_methods[i] == "GET" && current_method == HttpRequest::GET )
			return SUCCESS;
		else if ( allowed_methods[i] == "POST" && current_method == HttpRequest::POST )
			return SUCCESS;
		else if ( allowed_methods[i] == "DELETE" && current_method == HttpRequest::DELETE )
			return SUCCESS;
	}

	return ERROR;
}

void	ResponseBuilder::setHeaderLineFor405Error() {

	if ( code_ != 405 )
		return ;

	header_ << "Allow: ";

	const std::vector<std::string>&		allowed_methods = config_data_.getMethods();
	for ( std::vector<std::string>::size_type i = 0; i < allowed_methods.size(); i++ ) {
		header_ << allowed_methods[i] << " ";
	}
	header_ << Http::Formatting::CRLF;
}

const std::string	ResponseBuilder::readContentFromFile( const std::string& path ) {

	std::ifstream	content_stream( path.c_str(), std::ios::in | std::ios::binary );

	if ( content_stream.fail() ) {
		error_ = 500;
		return "";
	}
	int c = content_stream.get();
	if ( c == EOF )
		return "";

	std::string	content_string( 1, static_cast<char>(c) );
	while ( c != EOF ) {
		c = content_stream.get();
		if ( c == EOF )
			break;
		content_string += static_cast<char>(c);
	}

	return content_string;
}

const std::string	ResponseBuilder::getExtension( const std::string& path ) {

	size_t	point_position = path.find_last_of('.');
	if ( point_position == path.npos )
		return "No extension";


	return path.substr( point_position + 1 );
}

const std::string	ResponseBuilder::buildPathFromRootAndResource( std::string root, std::string resource ) {

	if ( resource == "" )
		return root;

	if ( resource[0] == '/' && root[root.size()-1] == '/' )
		root = root.substr(0, root.size()-1);
	else if ( resource[0] != '/' && root[root.size()-1] != '/' )
		root += "/";

	return root + resource;
}

const std::string	ResponseBuilder::cutPrefixFromUri( const std::string& uri_from_request ) {

	const std::string	prefix = config_data_.getPath();
	return uri_from_request.substr( prefix.length() );
}

DIR*	ResponseBuilder::openDirectory( const std::string& path ) {

	DIR*	dir_ptr = opendir( path.c_str() );
	if ( dir_ptr == NULL ) {
		setErrorState( 403 );
	}
	return dir_ptr;
}

void	ResponseBuilder::readDirectory( DIR* dir_ptr, std::vector<std::string>& files_from_dir ) {

	std::string	file_name;

	struct dirent*	read_from_dir = readdir( dir_ptr );
	while ( read_from_dir != NULL ) {

		file_name = read_from_dir->d_name;
		if ( file_name != "." && file_name != ".." ) {
			files_from_dir.push_back( file_name );
		}
		read_from_dir = readdir( dir_ptr );
	}
	closedir( dir_ptr );
}

int	ResponseBuilder::checkFilePermissions( const char* path ) {

	if ( access( path, R_OK ) == -1 ) {
		setErrorState( 403 );
		return ERROR;
	}
	return SUCCESS;
}

void	ResponseBuilder::setErrorState( int error_code ) {

	header_.str("");
	error_ = true;
	code_ = error_code;
}

std::string	ResponseBuilder::getElemFromMap( const std::map<std::string, std::string>& map, const std::string& first ) {

	const std::map<std::string, std::string>::const_iterator	it = map.find( first );
	if ( it != map.end() ) {
		return it->second;
	}

	return "";
}

void	ResponseBuilder::cutQueryFromUri( const std::string& uri ) {

	size_t position_of_delimiter = uri.find( "?" );
	if ( position_of_delimiter == uri.npos ) {
		uri_ = uri;
		return;
	}

	uri_ 	= uri.substr( 0, position_of_delimiter );
	query_  = uri.substr( position_of_delimiter + 1 );
}

std::string	ResponseBuilder::getCodeMeaning() {

	switch ( code_ ) {
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 204:
			return "No Content";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 303:
			return "See Other";
		case 307:
			return "Temporary Redirect";
		case 308:
			return "Permanent Redirect";
		case 400:
			return "Bad Request";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 413:
			return "Payload Too Large";
		case 415:
			return "Unsupported Media Type";
		case 500:
			return "Internal Server Error";
		case 502:
			return "Bad Gateway";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "Version Not Supported";
		default:
			return "This code is not implemented";
	}
}
