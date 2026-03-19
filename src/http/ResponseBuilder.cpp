/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/24 13:37:34 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> //opendir
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"

//"\e[1;92m"
//"\033[0m"
ResponseBuilder::ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data ) {

	error_ = false;
	file_or_dir_ = NOT_SPECIFIED;
	code_ = NOT_SPECIFIED;
	code_meaning_ = "";
	header_.str("");

	ResponseBuilder::setResponse( request, config_data );
	ResponseBuilder::setLastLineOfHeader();
}

ResponseBuilder::~ResponseBuilder() {}

const HttpResponse&	ResponseBuilder::build() {

	return response_;
}

void	ResponseBuilder::setResponse( const HttpRequest& request, const MergedConfig& config_data ) {

	config_data_ = config_data;
	if ( !config_data_.getReturn().empty() ) {
		ResponseBuilder::buildDirectReturn( config_data.getReturn() );
	}
	else {
		ResponseBuilder::buildResponseAccordingToMethod( request );
	}

	if ( error_ == true ) {
		ResponseBuilder::buildErrorResponse( code_ );
	}
}

void	ResponseBuilder::setLastLineOfHeader() {

	header_ << Http::Formatting::HEADER_END;
	response_.setHeader( header_ );
}

void	ResponseBuilder::buildDirectReturn( const std::map<int, std::string>& return_data ) {

	code_ = return_data.begin()->first;
	std::string	what_is_return = return_data.begin()->second;

	ResponseBuilder::setFirstLineOfHeader();

	if ( code_ >= 301 && code_ <= 308 ) {
		ResponseBuilder::setReturnRedirection( what_is_return );
	}
	else if ( what_is_return != "" && what_is_return[0] == '/' ) {
		ResponseBuilder::handleResource( what_is_return );
	}
	else {
		ResponseBuilder::setBasicReturn( what_is_return );
	}
}

void	ResponseBuilder::setFirstLineOfHeader() {

	code_meaning_ = ResponseBuilder::getCodeMeaning();
	if ( code_meaning_ == "This code is not implemented" ) {
		code_ = 500;
		code_meaning_ = ResponseBuilder::getCodeMeaning();
	}

	header_ << Http::Protocol::HTTP_VERSION_1_0 << " " << code_ << " " << code_meaning_ << Http::Formatting::CRLF;
}

void	ResponseBuilder::setReturnRedirection( const std::string& what_is_return ) {

	header_ << "Location: " << what_is_return << Http::Formatting::CRLF
			<< Http::Headers::CONTENT_LENGTH << ": 0";
}

void	ResponseBuilder::handleResource( const std::string resource ) {

	const std::string path = ResponseBuilder::buildPathFromRootAndResource( resource );
	if ( ResponseBuilder::checkIfPathExists( path ) == ERROR )
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

	if ( ResponseBuilder::checkFilePermissions( path.c_str() ) == ERROR )
		return ERROR;
	ResponseBuilder::setContentType( path );

	std::string	body;
	body = ResponseBuilder::readContentFromFile( path );
	if ( error_ == true ) {
		setErrorState( 500 );
		return ERROR;
	}
	response_.setBody( body );
	header_ << Http::Headers::CONTENT_LENGTH << ": " << response_.getBody().length();

	return SUCCESS;
}

void	ResponseBuilder::handleDirectory( const std::string& path_without_file ) {

	const std::vector<std::string>&	indices = config_data_.getIndex();
	for ( std::vector<std::string>::size_type i = 0; i < indices.size(); i++ ) {
		
		std::string	path = path_without_file + indices[i];
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
	
	buildListing( files_from_dir, path );
}

void	ResponseBuilder::setBasicReturn( const std::string& what_is_return ) {

	if ( what_is_return != "" )
		header_ << Http::Headers::CONTENT_TYPE << ": text/plain" << Http::Formatting::CRLF;

	header_ << Http::Headers::CONTENT_LENGTH << ": " << what_is_return.length();
	response_.setBody( what_is_return );
}

void	ResponseBuilder::setContentType( const std::string& path ) {

	std::string		content_type;
	const std::string	extension = ResponseBuilder::getExtension( path );

	if ( extension == "No extension" )
		content_type = "application/octet-stream";
	else if ( extension == "jpg" || extension == "jpeg" || extension == "png" )
		content_type = "image/" + extension;
	else
		content_type = "text/" + extension;

	header_ << Http::Headers::CONTENT_TYPE << ": " << content_type << Http::Formatting::CRLF;
}

void	ResponseBuilder::buildListing( std::vector<std::string>& files_from_dir, const std::string& path ) {

	header_ << Http::Headers::CONTENT_TYPE << ": " << "text/html" << Http::Formatting::CRLF;
	
	std::stringstream	body;
	body << "<!DOCTYPE html><html>"
		<< "<head><title>Index of " << path << "</title></head>"
		<< "<head>Index of " << path << "</head><br>"
		<< "<body>";
	for ( std::vector<std::string>::size_type i = 0; i < files_from_dir.size(); i++ ) {
		body << "<a href=" << path+files_from_dir[i] << ">" << files_from_dir[i] << "</a><br>";
	}
	body << "</body></html>";
	response_.setBody( body.str() );
	header_ << Http::Headers::CONTENT_LENGTH << ": " << response_.getBody().length();
}

int	ResponseBuilder::checkFilePermissions( const char* path ) {

	if ( access( path, R_OK ) == -1 ) {
		ResponseBuilder::setErrorState( 403 );
		return ERROR;
	}
	return SUCCESS;
}

void	ResponseBuilder::setErrorState( int error_code ) {

	header_.str("");
	error_ = true;
	code_ = error_code;
}

void	ResponseBuilder::buildResponseAccordingToMethod( const HttpRequest& request ) {

	if ( ResponseBuilder::checkMethodInRequest( request.getMethod(), config_data_.getMethods() ) == ERROR ) {

		ResponseBuilder::setErrorState( 405 );
		return ;
	}

	if ( request.getMethod() == HttpRequest::GET ) {

		ResponseBuilder::buildResponseGET( request );
	}
	else if ( request.getMethod() == HttpRequest::POST ) {

		if ( ResponseBuilder::checkBodySize( request.getContentLength(), config_data_.getMaxBodySize() ) == ERROR ) {
			ResponseBuilder::setErrorState( 413 );
			return ;
		}

		ResponseBuilder::buildResponsePOST( request );
	}
	else if ( request.getMethod() == HttpRequest::DELETE ) {

		ResponseBuilder::buildResponseDELETE( request );
	}

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

void	ResponseBuilder::buildResponseGET( const HttpRequest& request ) {

	code_ = 200;
	ResponseBuilder::setFirstLineOfHeader();
	std::string	path_without_prefix = cut_prefix_from_uri( request.getUri() );
	ResponseBuilder::handleResource( path_without_prefix );
}

int	ResponseBuilder::checkIfPathExists( const std::string& path ) {

	struct stat s;

	int result = stat( path.c_str(), &s );
	if ( result == -1 ) {
		ResponseBuilder::setErrorState( 404 );
		return ERROR;
	}

	if ( S_ISREG( s.st_mode ) != 0 ) {
		file_or_dir_ = IS_FILE;
	}
	else if ( S_ISDIR( s.st_mode ) != 0 ) {
		file_or_dir_ = IS_DIR;
	}
	else {
		ResponseBuilder::setErrorState( 403 );
		return ERROR;
	}

	return SUCCESS;
}

void	ResponseBuilder::buildResponsePOST( const HttpRequest& request ) {

	(void)request;

	header_ << "WIP: building POST response";
}

int	ResponseBuilder::checkBodySize( size_t current_body_size, size_t max_body_size ) {

	if ( current_body_size > max_body_size ) {

		return ERROR;
	}
	return SUCCESS;
}

void	ResponseBuilder::buildResponseDELETE( const HttpRequest& request ) {

	(void)request;
	header_ << "WIP: building DELETE response";
}

void	ResponseBuilder::buildErrorResponse( int code ) {

	code_ = code;

	ResponseBuilder::setFirstLineOfHeader();
	ResponseBuilder::addHeaderLineFor405Error();

	bool					error_page_from_config = false;
	const std::map<int, std::string>&	config_errors = config_data_.getErrorPage();
	for ( std::map<int, std::string>::const_iterator it = config_errors.begin(); it != config_errors.end(); it++ ) {
		if ( it->first == code_ ) {
			ResponseBuilder::setErrorPageHtml( it->second );
			if ( response_.getBody() != "" )
				error_page_from_config = true;
		}
	}
	if ( error_page_from_config == false )
		ResponseBuilder::setErrorPageHtml( "" );
}

void	ResponseBuilder::setErrorPageHtml( const std::string page_from_config ) {

	header_ << Http::Headers::CONTENT_TYPE << ": text/html" << Http::Formatting::CRLF;

	std::string	body;
	if ( page_from_config != "")
		body = ResponseBuilder::readContentFromFile( page_from_config );
	if ( body == "" )
		body = ResponseBuilder::generateDefaultPage();
	response_.setBody( body );

	header_ << Http::Headers::CONTENT_LENGTH << ": " << response_.getBody().length();
}

std::string	ResponseBuilder::generateDefaultPage() {

	std::stringstream	default_page;

	default_page << "<!DOCTYPE html><br>"
			<< "<html><br>"
			<< "<head><title>"
			<< code_ << code_meaning_
			<<"</title></head><br>"
			<< "<body><br>"
			<< "<center><h1>"
			<< code_ << code_meaning_
			<<"</h1></center><br>"
			<< "<hr><center> &#128225;&#9989; Webserv &#128029;</center><br>"
			<< "</body><br>"
			<< "</html>";
	return default_page.str();
}

void	ResponseBuilder::addHeaderLineFor405Error() {

	if ( code_ != 405 )
		return ;

	header_ << "Allow: ";

	const std::vector<std::string>&		allowed_methods = config_data_.getMethods();
	for ( std::vector<std::string>::size_type i = 0; i < allowed_methods.size(); i++ ) {
		header_ << allowed_methods[i] << " ";
	}
	header_ << Http::Formatting::CRLF;
}

std::string	ResponseBuilder::readContentFromFile( const std::string& path ) {

	std::ifstream	content_stream( path.c_str(), std::ios::in | std::ios::binary );

	if ( content_stream.fail() ) {
		error_ = 500;
		return "";
	}
	int c = content_stream.get();
//	std::cout << "\e[1;92m" << EOF << "\033[0m" << std::endl;
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

const std::string	ResponseBuilder::buildPathFromRootAndResource( const std::string file ) {

	std::string	root = config_data_.getRoot();

	if ( file == "" )
		return root;

	if ( file[0] == '/' && root[root.size()-1] == '/' )
		root = root.substr(0, root.size()-1);
	else if ( file[0] != '/' && root[root.size()-1] != '/' )
		root += "/";

	return root + file;
}

const std::string	ResponseBuilder::cut_prefix_from_uri( const std::string& uri_from_request ) {

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
		files_from_dir.push_back( file_name );

		read_from_dir = readdir( dir_ptr );
	}
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
