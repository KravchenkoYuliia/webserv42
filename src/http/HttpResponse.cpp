/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:24:00 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:07:07 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream> // ostringstream
#include "http/HttpConstants.hpp"
#include "http/HttpResponse.hpp"

HttpResponse::HttpResponse() {
	header_ = "";
	body_ = "";
}

HttpResponse::HttpResponse( const std::string& header, const std::string& body) {// delete if not used??
	header_ = header;
	body_ = body;
}

HttpResponse::~HttpResponse(void) {}

const std::string	HttpResponse::serialize() const {
	return header_ + body_;
}

//setters
//
void	HttpResponse::setHeader( const std::ostringstream& header ) {
	header_ = header.str();
}

void	HttpResponse::setBody( const std::string& body ) {
	body_ = body;
}


//getters
//
const std::string&	HttpResponse::getHeader() const {
	return header_;
}

const std::string&	HttpResponse::getBody() const {
	return body_;
}


/*


void    HttpResponse::setStatusCode(int code, const std::string& description)
{
    status_code_value_ = code;
    // TODO: should description be lowercase before saving it to simplify its potential future fetching?
    status_code_description_ = description;
}

void    HttpResponse::setHeader(const std::string& key, const std::string& value)
{
    headers_[key] = value;
}

void    HttpResponse::setBody(const std::string& body)
{
    body_ = body;
}

// --------------------------- Public Member Methods ---------------------------

// TODO: Delete this example used to hard-code the serialize skeleton

// std::string response =
    // "HTTP/1.1 200 OK\r\n"
    // "Content-Length: 24\r\n"
    // "Connection: close\r\n"
    // "\r\n"
    // "Hello pink t-shirts team";
std::string HttpResponse::serialize()
{
    std::ostringstream response;

    setStatusCode(200, "OK");

    response    << Http::Protocol::HTTP_VERSION_1_1
                << Http::Formatting::SPACE
                << status_code_value_
                << Http::Formatting::SPACE
                << status_code_description_
                << Http::Formatting::CRLF;

    for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); it++)
    {
        response    << it->first
                    << Http::Formatting::HEADER_SEPARATOR
                    << it->second
                    << Http::Formatting::CRLF;
    }
    response << Http::Formatting::CRLF;
    response << "Hello pink t-shirts team";
    return (response.str());
}*/
