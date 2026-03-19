/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/18 12:28:49 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream> //open
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"

//\e[1;92m

ResponseBuilder::ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data ) {
	
	config_data_ = config_data;
	if ( !config_data_.getReturn().empty() ) {//check RETURN - first priority
		ResponseBuilder::buildReturnResponse( config_data.getReturn() );
		return ;
	}
	if ( ResponseBuilder::checkMethodInRequest( request.getMethod(), config_data_.getMethods() ) == ERROR )
		return ;
	if ( request.getMethod() == HttpRequest::POST && ResponseBuilder::checkBodySize( request.getContentLength(), config_data_.getMaxBodySize() ) == ERROR )
		return ;
		
}

ResponseBuilder::~ResponseBuilder() {}

const HttpResponse&	ResponseBuilder::build() {

	return response_;
}

const HttpResponse&	ResponseBuilder::buildReturnResponse( const std::map<int, std::string>& return_data ) {

	code_ = return_data.begin()->first;
	std::string	what_is_return = return_data.begin()->second;

	ResponseBuilder::setFirstLineOfReturnResponse();

	if ( code_ >= 301 && code_ <= 308 ) {
		ResponseBuilder::buildReturnRedirection( what_is_return );
	}
	else if ( what_is_return != "" && what_is_return[0] == '/' ) {
		ResponseBuilder::buildReturnPageHtml( what_is_return );
	}
	else {	
		ResponseBuilder::buildBasicReturn( what_is_return );
	}

	header_ << Http::Formatting::HEADER_END;
	response_.setHeader( header_ );

	return response_;
}

void	ResponseBuilder::setFirstLineOfReturnResponse() {

	code_meaning_ = ResponseBuilder::getCodeMeaning();
	if ( code_meaning_ == "This code is not implemented" ) {
		code_ = 500;
		code_meaning_ = ResponseBuilder::getCodeMeaning();
	}

	header_ << Http::Protocol::HTTP_VERSION_1_0 << " " << code_ << " " << code_meaning_ << Http::Formatting::CRLF;
}

void	ResponseBuilder::buildReturnRedirection( const std::string& what_is_return ) {

	header_ << "Location: " << what_is_return << Http::Formatting::CRLF
			<< Http::Headers::CONTENT_LENGTH << ": 0";
}

void	ResponseBuilder::buildReturnPageHtml( const std::string what_is_return ) {

	header_ << Http::Headers::CONTENT_TYPE << ": text/html" << Http::Formatting::CRLF;

	std::string	body;
	if ( what_is_return != "")
		body = ResponseBuilder::readContentFromFile( what_is_return );
	if ( body == "" )
		body = ResponseBuilder::generateDefaultPage();
	response_.setBody( body );

	header_ << Http::Headers::CONTENT_LENGTH << ": " << response_.getBody().length();
}

void	ResponseBuilder::buildBasicReturn( const std::string& what_is_return ) {

	if ( what_is_return != "" )
		header_ << Http::Headers::CONTENT_TYPE << ": text/plain" << Http::Formatting::CRLF;

	header_ << Http::Headers::CONTENT_LENGTH << ": " << what_is_return.length();
	response_.setBody( what_is_return );
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

std::string	ResponseBuilder::generateDefaultPage() {

	std::stringstream	default_page;

	default_page << "<!DOCTYPE html>\n<html>\n<body>\n<h1>"
			<< code_
			<< "</h1>\n<p>"
			<< code_meaning_
			<< "</p>\n</body>\n</html>";

	return default_page.str();
}

std::string	ResponseBuilder::readContentFromFile( const std::string& file ) {

	std::ifstream		content_stream;
	const std::string	whole_path = ResponseBuilder::buildPathFromRootAndFile( file );
	std::cout << "Path is [" << whole_path << "]" << std::endl;

	content_stream.open( whole_path.c_str() );
	if ( content_stream.fail() || !content_stream.is_open() ) {
		std::cout << "Can't open file " << std::endl;
		return "";
	}
	std::string	buffer;
	std::string	content_string;
	getline( content_stream, buffer );
	while ( buffer != "" ) {
		content_string += buffer;
		if ( buffer != "</html>")
			content_string += "\n";

		getline( content_stream, buffer );
	}

	return content_string;
}

const std::string	ResponseBuilder::buildPathFromRootAndFile( const std::string& file ) {

	std::string	root = config_data_.getRoot();

	if ( file[0] == '/' && root[root.size()-1] == '/' )
		root = root.substr(0, root.size()-1);
	else if ( file[0] != '/' && root[root.size()-1] != '/' )
		root += "/";

	return root + file;
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
	ResponseBuilder::buildErrorResponse( 405 );
	return ERROR;
}

void	ResponseBuilder::buildErrorResponse( int code ) {

	code_ = code;
	ResponseBuilder::setFirstLineOfReturnResponse();
	ResponseBuilder::addHeaderLineFor405Error();
	
	bool					error_page_from_config = false;
	const std::map<int, std::string>&	config_errors = config_data_.getErrorPage();
	for ( std::map<int, std::string>::const_iterator it = config_errors.begin(); it != config_errors.end(); it++ ) {
		if ( it->first == code_ ) {
			ResponseBuilder::buildReturnPageHtml( it->second );
			if ( response_.getBody() != "" )
				error_page_from_config = true;
		}
	}
	if ( error_page_from_config == false )
		ResponseBuilder::buildReturnPageHtml( "" );

	header_ << Http::Formatting::HEADER_END;
	response_.setHeader( header_ );
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

int	ResponseBuilder::checkBodySize( size_t current_body_size, size_t max_body_size ) {

	if ( current_body_size > max_body_size ) {
		ResponseBuilder::buildErrorResponse( 413 );
		return ERROR;
	}
	return SUCCESS;
}

/*


HttpResponse    ResponseBuilder::build( const HttpRequest& request, const std::string &server, const std::string &uri )
{
    HttpResponse    response;

    (void)request;
    (void)server;
    (void)uri;
    return (response);
}

// TODO: Delete this example used to hard-code the build skeleton

// std::string response =
    // "HTTP/1.1 200 OK\r\n"
    // "Content-Length: 24\r\n"
    // "Connection: close\r\n"
    // "\r\n"
    // "Hello pink t-shirts team";
HttpResponse    ResponseBuilder::build(const HttpRequest& request)
{
    // TODO: check request.getMethod return value and create associated method type : buildGet / buildPost / buildDelete
    (void)request;

    HttpResponse    response;
    std::string     body = "Hello pink t-shirts team";

    response.setStatusCode(200, "OK");
    response.setHeader(Http::Headers::CONTENT_LENGTH, Utils::toString(body.length()));
    response.setHeader(Http::Headers::CONNECTION, "close");
    response.setHeader(Http::Headers::CONTENT_TYPE, "text/plain");
    response.setBody(body);
    return (response);
}*/
