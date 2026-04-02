/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:02:42 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 14:15:19 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cctype>   // isupper, tolower
#include <stdexcept>        //std::runtime_error
#include "http/HttpConstants.hpp"
#include "http/HttpRequest.hpp"
#include "utils/Utils.hpp"

// ############################# HttpRequest Class #############################

// ------------------------- Destructor / Constructor -------------------------

HttpRequest::HttpRequest(void)
    :   method_(GET), // TODO: is not hiding that default method is GET?
        version_(Http::Protocol::HTTP_VERSION_1_1),
        content_length_(0),
        chunk_size_(0),
        is_multipart_(false)
{
}

HttpRequest::~HttpRequest(void)
{
    // TODO: Delete this log
    std::cout << "HttpRequest destructor called" << std::endl;
}

HttpRequest::HttpRequest(const HttpRequest& copy)
{
    *this = copy;
}

HttpRequest& HttpRequest::operator=(const HttpRequest& copy)
{
    if (this != &copy)
    {
        method_ = copy.method_;
        version_ = copy.version_;
        content_length_ = copy.content_length_;
        chunk_size_ = copy.chunk_size_;
        is_multipart_ = copy.is_multipart_;
    }
    return (*this);
}

// --------------------------- Public Setter Methods ---------------------------

void    HttpRequest::setMethod(HttpRequest::Method method)
{
    method_ = method;
}

void    HttpRequest::setMethod(const std::string& method)
{
    if (method == Http::Method::GET)
        method_ = GET;
    else if (method == Http::Method::POST)
        method_ = POST;
    else if (method == Http::Method::DELETE)
        method_ = DELETE;
    else
        method_ = UNKNOWN;

}

void    HttpRequest::setUri(const std::string& uri)
{
    // TODO: consider adding an uri limit to handle 414 - request uri too long?
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

void	HttpRequest::setCookie(const std::string& key, const std::string& value)
{
	std::string normalized_key = Utils::toLower(key);
	cookie_[normalized_key] = value;
}

void    HttpRequest::appendToBody(const std::string& body)
{
    body_.append(body);
}

void    HttpRequest::setContentLength(size_t value)
{
    content_length_ = value;
}

void    HttpRequest::setChunkSize(size_t value)
{
    chunk_size_ = value;
}

void    HttpRequest::setIsMultipart(bool value)
{
    is_multipart_ = value;
}

void HttpRequest::setMultipartData(const MultipartData& data)
{
    multipart_data_ = data;
    is_multipart_ = true;
}


// --------------------------- Public Getter Methods ---------------------------

HttpRequest::Method                             HttpRequest::getMethod() const
{
    return (method_);
}

std::string                             HttpRequest::getMethodToString() const
{
    if (method_ == GET)
        return (Http::Method::GET);
    else if (method_ == POST)
        return (Http::Method::POST);
    else if (method_ == DELETE)
        return (Http::Method::DELETE);
    return (Http::Method::UNKNOWN);
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

const std::map<std::string, std::string>&       HttpRequest::getCookie() const
{
    return (cookie_);
}

const std::string&                                     HttpRequest::getHeaderValue(const std::string& key) const
{
    static const std::string empty_string = "";
    const std::string normalize_key = Utils::toLower(key);
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

bool                                            HttpRequest::getIsMultipart() const
{
    return (is_multipart_);
}


const MultipartData& HttpRequest::getMultipartData() const
{
    return (multipart_data_);
}

// --------------------------- Public Member Methods ---------------------------

bool    HttpRequest::hasHeader( const std::string& key) const
{
    std::string normalized_key = Utils::toLower(key);
    return (headers_.find(normalized_key) != headers_.end());
}

bool HttpRequest::isCgiRequest(const std::map<std::string, std::string>& cgi_map) const
{
    // const std::map<std::string,std::string>& cgi_map = config.getCgi();
    if (cgi_map.empty())
        return (false);
    const std::string& uri = getUri();
    size_t dot_pos = uri.find_last_of('.');
    if (dot_pos == std::string::npos)
        return (false);
    std::string extension = uri.substr(dot_pos);
    return (cgi_map.find(extension) != cgi_map.end());
}
