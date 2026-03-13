/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 12:54:51 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"
#include "utils/Utils.hpp"

ResponseBuilder::ResponseBuilder(void)
{
    // TODO: Delete this log

    std::cout << "ResponseBuilder default constructor called" << std::endl;
}

ResponseBuilder::~ResponseBuilder(void)
{
    // TODO: Delete this log

    std::cout << "ResponseBuilder destructor called" << std::endl;
}

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
}
