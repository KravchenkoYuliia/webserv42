/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:07 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 13:22:42 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib> // strtol
#include <errno.h> // errno
#include "http/HttpConstants.hpp"
#include "http/RequestParser.hpp"
#include "utils/Utils.hpp"

// ------------------------- Struct ParserState helper -------------------------

std::string     RequestParser::ParserState::toString(RequestParser::ParserState::Type state)
{
    switch (state)
    {
        case ParserState::REQUEST_LINE:
            return "REQUEST_LINE";
        case ParserState::HEADERS:
            return "HEADERS";
        case ParserState::BODY_CONTENT_LENGTH:
            return "BODY_CONTENT_LENGTH";
        case ParserState::BODY_CHUNKED:
            return "BODY_CHUNKED";
        case ParserState::BODY_NONE:
            return "BODY_NONE";
        case ParserState::COMPLETE:
            return "COMPLETE";
        case ParserState::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

// ############################ RequestParser Class ############################

// ------------------------- Destructor / Constructor -------------------------

RequestParser::RequestParser(void)
    :   request_(),
        response_(),
        state_(RequestParser::ParserState::REQUEST_LINE)
{
    std::cout << "RequestParser default constructor called" << std::endl;
}

RequestParser::~RequestParser(void)
{
    std::cout << "RequestParser destructor called" << std::endl;
}

// --------------------------- Public Getter Methods ---------------------------

const HttpRequest&     RequestParser::getRequest() const
{
    return (request_);
}

std::string  RequestParser::getHeader( const std::string& key ) const
{
    return (request_.getHeader(key));
}

const std::string&  RequestParser::getUri() const
{
    return (request_.getUri());
}

// --------------------------- Public Member Methods ---------------------------

void    RequestParser::appendData(const char *data, size_t len)
{
    raw_buffer_.append(data, len);
}

bool    RequestParser::isComplete() const
{
    return (state_ == ParserState::COMPLETE);
}

// see example from nginx: https://sources.debian.org/src/nginx/1.28.2-2/src/http/ngx_http_parse.c
// see example from nodejs: https://github.com/nodejs/llhttp/blob/main/src/native/http.c
bool RequestParser::parseRequest(const std::string& request)
{
    // TODO: extract the logic from ConectionHandler::handleRead here ?
    (void)request;
    return (true);
}

RequestParser::ResultType       RequestParser::parseNext()
{
    switch (state_)
    {
        case ParserState::REQUEST_LINE:
            return (parseRequestLine());
        case ParserState::HEADERS:
            return (parseHeaders());
        case ParserState::BODY_CONTENT_LENGTH:
            return (parseBodyContentLength());
        case ParserState::BODY_CHUNKED:
            return (parseBodyChunked());
        case ParserState::BODY_NONE:
        {
            state_ = ParserState::COMPLETE; // TODO: check if not redundant with validateHeaders()
            return (ParserResult::OK);
        }
        case ParserState::COMPLETE:
            return (ParserResult::OK);
        case ParserState::ERROR:
            return (ParserResult::ERROR);
    }
    return (ParserResult::ERROR);
}


// -------------------------- Private Member Methods --------------------------

RequestParser::ResultType       RequestParser::parseRequestLine()
{
    if (!hasEndOfLine())
        return (ParserResult::AGAIN);

    std::string     line = extract_line();

    if (!parseRequestLineFields(line))
    {
        state_ = ParserState::ERROR;
        return (ParserResult::ERROR);
    }
    state_ = ParserState::HEADERS;
    return (ParserResult::OK);
}

RequestParser::ResultType       RequestParser::parseHeaders()
{
    if (!hasEndOfLine())
        return (ParserResult::AGAIN);

    std::string     line = extract_line();

    if (line.empty())
    {
        if (!validateHeaderSet())
        {
            // TODO: remove this log
            std::cout << "validateHeaderSet return false because method = " << request_.getMethodToString() << ", state_ = " << ParserState::toString(state_) << std::endl;
            state_ = ParserState::ERROR;
            return (ParserResult::ERROR);
        }
        return (ParserResult::OK);
    }
    if (!parseHeaderLine(line))
    {
        // TODO: remove this log
        std::cout << "parseHeaderLine return false" << std::endl;
        state_ = ParserState::ERROR;
        return (ParserResult::ERROR);
    }

    return (ParserResult::OK);
}

bool RequestParser::validateHeaderSet()
{
    // 1: Check presence of HTTP mention + version

    if (request_.getVersion() == Http::Protocol::HTTP_VERSION_1_1)
    {
        if (!request_.hasHeader(Http::Headers::HOST))
        {
            std::cout << "has headers failed" << std::endl;
            return (false);
        }
    }

    // 2 : Check Headers conflicts
    /*
        RFC-7230 - section 3.3.2 "A sender MUST NOT send a Content-Length header field in any message
        that contains a Transfer-Encoding header field."
    */
    // TODO: see how to return an ERROR then ste status cod eto 400 when if (hasContentLength && hasTransferEncoding)

    std::string cl = request_.getHeader(Http::Headers::CONTENT_LENGTH);
    std::string te = request_.getHeader(Http::Headers::TRANSFER_ENCODING);

    if (!cl.empty() && !te.empty())
    {
        std::cout << " if !cl.empty() && !te.empty() return false" << std::endl;
        return (false);
    }

    if (!te.empty())
    {
        if (te != "chunked") // TODO: create variable to hold "chunk"
        {
            // TODO: remove this log
            std::cout << " if te != chunked return false " << std::endl;
            return (false);
        }
        request_.setChunkSize(0);
        state_ = ParserState::BODY_CHUNKED;
    }
    else if (!cl.empty())
    {
        // TODO: extract usage of std::strtol into helper
        // TODO: check which one if can be interesting to use std::strtoll or std::strtoull instead of std::strtol
        char *endptr;
        errno = 0;
        long value = std::strtol(cl.c_str(), &endptr, 10);
        // TODO: can i check this errno here?
        if (errno != 0 || *endptr != '\0' || value < 0)
            return false;
        request_.setContentLength(static_cast<size_t>(value));
        if (request_.getContentLength() > 0)
            state_ = ParserState::BODY_CONTENT_LENGTH;
        else
            state_ = ParserState::COMPLETE;
    }
    else
        state_ = ParserState::BODY_NONE;

    // 3. Check Body state with method type

    if (request_.getMethod() == HttpRequest::GET && state_ != ParserState::BODY_NONE)
    {
        // TODO: remove this log
        std::cout << "Failed because method = " << request_.getMethod() << ", state_ = " << state_ << std::endl;
        return (false);
    }
    return (true);

    // TODO: any others check to perform on this step?
}

bool    RequestParser::parseHeaderLine( const std::string& line )
{
    static const std::string    delimiter = Http::Formatting::COLON_SEPARATOR;
    size_t                      pos = line.find(delimiter);

    if (pos == std::string::npos)
        return (false);

    std::string     key = line.substr(0, pos);
    std::string     value = line.substr(pos + delimiter.length());

    request_.setHeader(Utils::trim(key), Utils::trim(value));
    return (true);
}

RequestParser::ResultType       RequestParser::parseBodyContentLength()
{
    // TODO: to finish
    return (ParserResult::OK);
}

RequestParser::ResultType       RequestParser::parseBodyChunked()
{
    // TODO: to finish
    return (ParserResult::OK);
}


bool    RequestParser::hasEndOfLine() const
{
    return (findCRLF() != std::string::npos);
}

std::string RequestParser::extract_line()
{
    size_t      pos = findCRLF();
    std::string line = raw_buffer_.substr(0, pos);
    size_t      CRLF_size = 2;

    raw_buffer_.erase(0, pos + CRLF_size);
    return (line);
}

bool RequestParser::parseRequestLineFields( const std::string& line )
{
    std::vector<std::string>    tokens_list;
    std::string                 token;
    std::istringstream          iss(line);

    while (iss >> token)
        tokens_list.push_back(token);
    if (tokens_list.size() != 3)
        return (false);
    request_.setMethod(tokens_list[0]);
    // TODO: add check on URI et Version??
    // TODO: only allow http 1.1 and http 1.0
    request_.setUri(tokens_list[1]);
    request_.setVersion(tokens_list[2]);
    return (true);
}

size_t    RequestParser::findCRLF() const
{
    return (raw_buffer_.find(Http::Formatting::CRLF));
}
