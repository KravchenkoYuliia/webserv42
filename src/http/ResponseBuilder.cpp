/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/26 11:55:41 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> //opendir
#include <ctime>
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"

//std::cout << "\e[1;92m" << "\033[0m" << std::endl;
ResponseBuilder::ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data ) {

	initialize_values( request.getUri(), config_data );
	
	/*if ( request.getErrorCode() != NOT_SPECIFIED ) {
		setErrorState( request.getErrorCode() );
	}
	else {*/
		buildResponse( request );
	//}

	if ( error_ == true ) {
		buildErrorResponse();
	}
	
	setContentLength();
	setLastLineOfHeader();
}

ResponseBuilder::~ResponseBuilder() {}

void	ResponseBuilder::buildResponse( const HttpRequest& request ) {
	
	if ( !config_data_.getReturn().empty() ) {
		buildReturn( config_data_.getReturn() );
	}
	else {
		buildResponseAccordingToMethod( request );
	}
}

void	ResponseBuilder::initialize_values( const std::string& uri, const MergedConfig& config_data ) {

	error_ = false;
	file_or_dir_ = NOT_SPECIFIED;
	code_ = NOT_SPECIFIED;
	code_meaning_ = "";
	header_.str("");

	config_data_ = config_data;
	uri_ = uri;
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
	response_.setBody( generateDefaultPage() );
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

	(void)request;

	header_ << "WIP: building POST response";
}

void	ResponseBuilder::buildResponseDELETE( const HttpRequest& request ) {

	(void)request;
	header_ << "WIP: building DELETE response";
}

void	ResponseBuilder::buildErrorResponse() {

	setStatusCode();
	setHeaderLineFor405Error();
	setServerAndDate();

	bool					error_page_from_config = false;
	const std::map<int, std::string>&	config_errors = config_data_.getErrorPage();
	for ( std::map<int, std::string>::const_iterator it = config_errors.begin(); it != config_errors.end(); it++ ) {
		if ( it->first == code_ ) {
			setErrorPageHtml( it->second );
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
	if ( error_ == true ) {
		setErrorState( 500 );
		return ERROR;
	}

	response_.setBody( body );
	return SUCCESS;
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
		body = generateDefaultPage();
	response_.setBody( body );
}

const std::string	ResponseBuilder::generateDefaultPage() {

	std::stringstream	default_page;

	default_page << "<!DOCTYPE html>\n"
			<< "<html>\n"
			<< "<head><title>"
			<< code_ << " " << code_meaning_
			<<"</title></head><br>\n"
			<< "<body>\n"
			<< "<center><h1>"
			<< code_ << " " << code_meaning_
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
