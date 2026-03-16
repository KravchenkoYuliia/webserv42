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
	
	(void)request;

	config_data_ = config_data;
	if ( !config_data_.getReturn().empty() ) {//check RETURN - first priority
		ResponseBuilder::buildReturnResponse( config_data.getReturn() );
	}

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

void	ResponseBuilder::buildReturnPageHtml( const std::string& what_is_return ) {

	header_ << Http::Headers::CONTENT_TYPE << ": text/html" << Http::Formatting::CRLF;

	std::string	body = ResponseBuilder::readContentFromFile( what_is_return );
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

	content_stream.open( whole_path.c_str() );
	if ( content_stream.fail() || !content_stream.is_open() ) {
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
	return root + file;
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
