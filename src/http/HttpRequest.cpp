/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:02:42 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:05:25 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cctype>   // isupper, tolower
#include "http/HttpConstants.hpp"
#include "http/HttpRequest.hpp"
#include "utils/Utils.hpp"

// ############################# HttpRequest Class #############################

// ------------------------- Destructor / Constructor -------------------------

HttpRequest::HttpRequest(void)
    :   method_(GET), // TODO: is not hiding that default method is GET?
        version_(Http::Protocol::HTTP_VERSION_1_1), // TODO: should we use default 1.1?
        content_length_(0),
        chunk_size_(0)
{
    // TODO: Delete this log
    std::cout << "HttpRequest default constructor called" << std::endl;
}

HttpRequest::~HttpRequest(void)
{
    // TODO: Delete this log
    std::cout << "HttpRequest destructor called" << std::endl;
}

// --------------------------- Public Setter Methods ---------------------------

void    HttpRequest::setMethod(HttpRequest::Method method)
{
    method_ = method;
}

void    HttpRequest::setMethod(const std::string& method)
{
    if (method == "GET")
        method_ = GET;
    else if (method == "POST")
        method_ = POST;
    else if (method == "DELETE")
        method_ = DELETE;
    else
        method_ = UNKNOWN;

}

void    HttpRequest::setUri(const std::string& uri)
{
    uri_ = uri;
}

void    HttpRequest::setVersion(const std::string& version)
{
    version_ = version;
}

void    HttpRequest::setHeader(const std::string& key, const std::string& value)
{
    /*
        RFC 7230 - section 3.2
        Each header field consists of a case-insensitive field name followed
        by a colon (":"), optional leading whitespace, the field value, and
        optional trailing whitespace.
    */
    std::string normalized_key = Utils::toLower(key);
    headers_[normalized_key] = value;
}

void    HttpRequest::setBody(const std::string& body)
{
    body_ = body;
}

void    HttpRequest::setContentLength(size_t value)
{
    content_length_ = value;
}
void    HttpRequest::setChunkSize(size_t value)
{
    chunk_size_ = value;
}

// --------------------------- Public Getter Methods ---------------------------

HttpRequest::Method                             HttpRequest::getMethod() const
{
    return (method_);
}

std::string                             HttpRequest::getMethodToString() const
{
    if (method_ == GET)
        return ("GET");
    else if (method_ == POST)
        return ("POST");
    else if (method_ == DELETE)
        return ("DELETE");
    return ("UNKNOWN");
}

const std::string&                              HttpRequest::getUri() const
{
    return (uri_);
}

const std::string&                              HttpRequest::getVersion() const
{
    return (version_);
}

const std::map<std::string, std::string>&       HttpRequest::getHeaders() const
{
    return (headers_);
}

std::string                                     HttpRequest::getHeader(const std::string& key) const
{
    std::string empty_string = "";
    std::string normalize_key = Utils::toLower(key);
    std::map<std::string, std::string>::const_iterator    it = headers_.find(normalize_key);
    if (it != headers_.end())
        return (it->second);
    else
        return (empty_string);
}

const std::string&                              HttpRequest::getBody() const
{
    return (body_);
}

size_t                                          HttpRequest::getContentLength() const
{
    return (content_length_);
}

size_t                                          HttpRequest::getChunkSize() const
{
    return (chunk_size_);
}

// --------------------------- Public Member Methods ---------------------------

bool    HttpRequest::hasHeader( const std::string& key) const
{
    // TODO: Delete this log
    std::cout << "hasHeader: key = " << key << std::endl;

    std::string normalized_key = Utils::toLower(key);
    return (headers_.find(normalized_key) != headers_.end());
}
