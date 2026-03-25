/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:12:07 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/26 15:45:26 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>      // strlen
#include <algorithm>    // std::min
#include <cstdlib>      // strtoul
#include <errno.h>      // errno
#include "http/MultipartParser.hpp"
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
        state_(RequestParser::ParserState::REQUEST_LINE),
        error_code_(-1),
        content_length_bytes_(0),
        current_chunk_size_(0),
        waiting_for_chunk_size_(true)
{
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

const std::string&  RequestParser::getHeaderValue( const std::string& key ) const
{
    return (request_.getHeaderValue(key));
}

const std::string&  RequestParser::getUri() const
{
    return (request_.getUri());
}

int RequestParser::getErrorCode() const
{
    return (error_code_);
}

// --------------------------- Public Member Methods ---------------------------

void    RequestParser::appendData( const char *data, size_t len )
{
    raw_buffer_.append(data, len);
}

bool    RequestParser::isComplete() const
{
    return (state_ == ParserState::COMPLETE);
}

// see example from nginx: https://sources.debian.org/src/nginx/1.28.2-2/src/http/ngx_http_parse.c
// see example from nodejs: https://github.com/nodejs/llhttp/blob/main/src/native/http.c
bool RequestParser::parseRequest( const std::string& request )
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
            return (parseHeaderFields());
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

    std::string     line = extract_header_field();

    if (!parseRequestLineFields(line))
    {
        std::cout << "parseRequestLineFields(line) failed" << std::endl;
        state_ = ParserState::ERROR;
        return (ParserResult::ERROR);
    }
    state_ = ParserState::HEADERS;
    return (ParserResult::OK);
}

RequestParser::ResultType       RequestParser::parseHeaderFields()
{
    if (!hasEndOfLine())
        return (ParserResult::AGAIN);

    std::string     extracted_header_field = extract_header_field();

    if (extracted_header_field.empty())
    {
        if (!validateHeaderFields())
        {
            // TODO: remove this log
            std::cout << "validateHeaderFields return false because method = " << request_.getMethodToString() << ", state_ = " << ParserState::toString(state_) << std::endl;
            state_ = ParserState::ERROR;
            return (ParserResult::ERROR);
        }
        return (ParserResult::OK);
    }
    if (!parseHeaderField(extracted_header_field))
    {
        // TODO: remove this log
        std::cout << "parseHeaderField return false" << std::endl;
        // error_code_ = 400;
        state_ = ParserState::ERROR;
        return (ParserResult::ERROR);
    }

    return (ParserResult::OK);
}

bool RequestParser::validateHeaderFields()
{
    if (!validateHostHeader())
    {
        std::cout << "validateHostHeader() failed" << std::endl;
        return (false);
    }
    // 2 : Check Headers conflicts
    /*
        RFC-7230 - section 3.3.2 "A sender MUST NOT send a Content-Length header field in any message
        that contains a Transfer-Encoding header field."
    */
    if (!validateHeaderConflicts())
    {
        std::cout << "validateHeaderConflicts failed" << std::endl;
        return (false);
    }
    if (!validateTransferEncodingHeader())
    {
        std::cout << "validateTransferEncodingHeader failed" << std::endl;
        return (false);
    }
    if (!validateContentTypeHeader())
    {
        std::cout << "validateContentTypeHeader failed" << std::endl;
        return (false);
    }
    if (!validateContentLengthHeader())
    {
        std::cout << "validateContentLengthHeader failed" << std::endl;
        return (false);
    }
    if (!validateBodyForMethod())
    {
        std::cout << "validateBodyForMethod failed" << std::endl;
        return (false);
    }
    // TODO: any others check to perform on this step?
    return (true);
}

/**
 * @brief Parses and stores a single HTTP header field.
 *
 * This method extracts the header key and value from a raw header line
 * using the ':' delimiter, trims surrounding whitespace, and validates
 * the result before storing it in the request object.
 *
 * Behavior:
 * - Splits the input line into key and value at the first ':' occurrence.
 * - Trims leading and trailing whitespace from both key and value.
 * - Ensures the key is not empty.
 * - Verifies header uniqueness constraints via validateHeaderFieldIsUnique().
 * - Stores the header in the request object if valid.
 *
 * @param line The raw header line to parse (e.g., "Host: example.com").
 *
 * @return true if the header field is successfully parsed and stored.
 * @return false if:
 *         - The delimiter ':' is missing,
 *         - The header key is empty,
 *         - The header violates uniqueness constraints.
 *
 * @note On failure due to uniqueness violation, error_code_ may be set
 *       by validateHeaderFieldIsUnique().
 */
bool    RequestParser::parseHeaderField( const std::string& line )
{
    static const std::string    delimiter = Http::Formatting::COLON_SEPARATOR;
    size_t                      pos = line.find(delimiter);

    if (pos == std::string::npos)
        return (false);

    std::string     key = Utils::trim( line.substr(0, pos) );
    std::string     value = Utils::trim( line.substr(pos + delimiter.length()) );
    if (key.empty())
        return (false);
    if (!validateHeaderFieldIsUnique(key, value))
        return (false);
    request_.setHeader(key, value);
    return (true);
}

/**
 * @brief Validates that a header field complies with uniqueness constraints.
 *
 * This method ensures that certain HTTP headers are not duplicated in a way
 * that violates the HTTP specification or internal parser rules.
 *
 * Rules enforced:
 * - "Content-Length":
 *   - May appear multiple times only if all values are identical.
 *   - If a different value is encountered, the request is invalid.
 *
 * - "Host", "Content-Type", "Transfer-Encoding":
 *   - Must not appear more than once.
 *
 * Header name comparison is performed case-insensitively.
 *
 * @param key The header field name.
 * @param value The header field value.
 *
 * @return true if the header field respects uniqueness constraints.
 * @return false if a violation is detected.
 *
 * @note On failure, sets error_code_ to 400 (Bad Request).
 */
bool            RequestParser::validateHeaderFieldIsUnique( const std::string& key, const std::string& value )
{
    const std::string normalized_key = Utils::toLower(key);
    static const std::string normalized_host = Utils::toLower(Http::Headers::HOST);
    static const std::string normalized_content_length = Utils::toLower(Http::Headers::CONTENT_LENGTH);
    static const std::string normalized_transfer_encoding = Utils::toLower(Http::Headers::TRANSFER_ENCODING);
    static const std::string normalized_content_type = Utils::toLower(Http::Headers::CONTENT_TYPE);

    if ( normalized_key == normalized_content_length && request_.hasHeader(normalized_key))
    {
        if (request_.getHeaderValue(normalized_key) != value)
        {
            error_code_ = 400;
            return (false);
        }
    }

    if ((normalized_key == normalized_host
            || normalized_key == normalized_content_type
            || normalized_key == normalized_transfer_encoding)
            && request_.hasHeader(normalized_key))
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    return (true);
}

RequestParser::ResultType       RequestParser::parseBodyContentLength()
{
    if (raw_buffer_.empty())
        return (ParserResult::AGAIN);
    size_t              body_data_size_parsed = std::min(raw_buffer_.size(), content_length_bytes_);
    const std::string   body_data = raw_buffer_.substr(0, body_data_size_parsed);
    request_.appendToBody(body_data);
    raw_buffer_.erase(0, body_data_size_parsed);
    content_length_bytes_ -= body_data_size_parsed;
    if (content_length_bytes_ == 0)
    {
        state_ = ParserState::COMPLETE;
        handleMultiPart();
        if (state_ == ParserState::ERROR)
            return (ParserResult::ERROR);
        return (ParserResult::OK);
    }
    return (ParserResult::AGAIN);
}

RequestParser::ResultType       RequestParser::parseBodyChunked()
{
    size_t CRLF_size = std::strlen(Http::Formatting::CRLF);
    while (true)
    {
        if (waiting_for_chunk_size_)
        {
            if (!hasEndOfLine())
                return (ParserResult::AGAIN);
            std::string     line = extract_header_field();
            line = Utils::trim(line);
            bool            success;
            int             base = 16;
            size_t          separator = line.find(';');
            if (separator != std::string::npos)
                line = line.substr(0, separator);
            current_chunk_size_ = parseUnsignedLong(line, success, base);
            if (!success)
            {
                error_code_ = 400;
                state_ = ParserState::ERROR;
                return (ParserResult::ERROR);
            }
            if (current_chunk_size_ == 0)
            {
                if (raw_buffer_.size() < CRLF_size)
                    return (ParserResult::AGAIN);
                if (raw_buffer_.substr(0, 2) != Http::Formatting::CRLF)
                {
                    error_code_ = 400;
                    state_ = ParserState::ERROR;
                    return (ParserResult::ERROR);
                }
                raw_buffer_.erase(0, CRLF_size);
                handleMultiPart();
                if (state_ == ParserState::ERROR)
                    return (ParserResult::ERROR);
                state_ = ParserState::COMPLETE;
                return (ParserResult::OK);
            }
            waiting_for_chunk_size_ = false;
        }
        size_t total_chunk_size = current_chunk_size_ + CRLF_size;
        if (raw_buffer_.size() < total_chunk_size)
            return (ParserResult::AGAIN);

        std::string     chunk = raw_buffer_.substr(0, current_chunk_size_);
        request_.appendToBody(chunk);
        raw_buffer_.erase(0, total_chunk_size);

        waiting_for_chunk_size_ = true;
        current_chunk_size_ = 0;
    }
    return (ParserResult::OK);
}

bool    RequestParser::hasEndOfLine() const
{
    return (findCRLF() != std::string::npos);
}

std::string RequestParser::extract_header_field()
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

    if (!isValidMethod(tokens_list[0]) || !isValidUriFormat(tokens_list[1]))
    {
        std::cout << "!isValidMethod(tokens_list[0]) || !isValidUriFormat(tokens_list[1]) failed! method = " << tokens_list[0] << " uri format = " << tokens_list[1] << std::endl;
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    request_.setMethod(tokens_list[0]);
    request_.setUri(tokens_list[1]);

    if (!isValidHttpProtocolVersion(tokens_list[2]))
    {
        std::cout << "isValidHttpProtocolVersion(tokens_list[2]) failed! http protocol version = " << tokens_list[2] << std::endl;
        error_code_ = 505; // Todo: // HTTP_VERSION_NOT_SUPPORTED
        return (false);
    }
    request_.setVersion(tokens_list[2]);
    return (true);
}

// ------------------------------- Method Helper -------------------------------

// TODO: replace with ParserUtils::findCRLF()
size_t    RequestParser::findCRLF() const
{
    return (raw_buffer_.find(Http::Formatting::CRLF));
}

bool RequestParser::isValidMethod( const std::string& method ) {
    return (method == Http::Method::GET || method == Http::Method::POST || method == Http::Method::DELETE);
}

/**
 * @brief Checks whether a URI is syntactically valid for an HTTP request line.
 *
 * This function ensures that the URI:
 *  - Is not empty.
 *  - Starts with a '/' character.
 *  - Does not contain literal spaces or tabs (only percent-encoded characters like %20 are allowed).
 *
 * @param uri The URI string to validate.
 * @return true If the URI is non-empty, starts with '/', and contains no literal spaces or tabs.
 * @return false Otherwise, indicating an invalid URI for the HTTP request line.
 */
bool    RequestParser::isValidUriFormat( const std::string& uri )
{
    if (uri.empty() || uri[0] != '/')
        return (false);
    // reject literal spaces
    for (size_t i = 0; i < uri.size(); ++i)
    {
        if (uri[i] == ' ' || uri[i] == '\t')
        {
            std::cout << "uri[i] == ' ' || uri[i] == '\t' failed" << std::endl;
            return (false);
        }
    }
    return (true);
}

bool    RequestParser::isValidHttpProtocolVersion( const std::string& protocol_version )
{
    return (protocol_version == Http::Protocol::HTTP_VERSION_1_0 || protocol_version == Http::Protocol::HTTP_VERSION_1_1);
}

bool    RequestParser::validateHostHeader()
{
    if ( request_.getVersion() == Http::Protocol::HTTP_VERSION_1_1 && !request_.hasHeader( Http::Headers::HOST ) )
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    return (true);
}

bool    RequestParser::validateHeaderConflicts()
{
    if (!request_.getHeaderValue(Http::Headers::CONTENT_LENGTH).empty()
            && !request_.getHeaderValue(Http::Headers::TRANSFER_ENCODING).empty())
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    return (true);
}

bool    RequestParser::validateTransferEncodingHeader()
{
    const std::string& transfer_encoding_header = request_.getHeaderValue(Http::Headers::TRANSFER_ENCODING);

    if (transfer_encoding_header.empty())
        return (true);

    const std::string transfer_encoding_type = "chunked";
    if (transfer_encoding_header != transfer_encoding_type)
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    request_.setChunkSize(0);
    state_ = ParserState::BODY_CHUNKED;
    return (true);
}

bool    RequestParser::validateContentLengthHeader()
{
    const std::string& content_length_header = request_.getHeaderValue(Http::Headers::CONTENT_LENGTH);

    if (content_length_header.empty())
    {
        if (state_ == ParserState::BODY_CHUNKED)
            return (true);
        state_ = ParserState::BODY_NONE;
        return (true);
    }

    bool success;
    int base = 10;
    unsigned long value = parseUnsignedLong(content_length_header, success, base);
    if (!success)
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }

    try {
        request_.setContentLength(static_cast<size_t>(value));
    } catch (const std::runtime_error& e)
    {
        error_code_ = 413; // TODO: PAYLOAD_TOO_LARGE
        return (false);
    }
    if (value == 0)
    {
        state_ = ParserState::COMPLETE;
        return (true);
    }
    state_ = ParserState::BODY_CONTENT_LENGTH;
    content_length_bytes_ = value;
    return (true);
}

bool    RequestParser::validateBodyForMethod()
{
    if (request_.getMethod() == HttpRequest::GET && state_ != ParserState::BODY_NONE)
    {
        error_code_ = 400; // TODO: BAD_REQUEST
        return (false);
    }
    return (true);
}

bool    RequestParser::validateContentTypeHeader()
{
    const std::string& content_type = request_.getHeaderValue(Http::Headers::CONTENT_TYPE);

    if (content_type.empty())
        return (true);
    if (content_type.find(Http::ContentType::TEXT_PLAIN) != std::string::npos)
        return (true);
    if (content_type.find(Http::ContentType::MULTIPART_FORM_DATA) != std::string::npos)
    {
        request_.setIsMultipart(true);
        return (true);
    }
    if (content_type.find(Http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED) != std::string::npos)
        return (true);
    error_code_ = 415; // TODO: UNSUPPORTED_MEDIA_TYPE
    return (false);
}

unsigned long RequestParser::parseUnsignedLong(const std::string &str, bool &success, int base)
{
    char            *endptr = 0;
    unsigned long   value = std::strtoul(str.c_str(), &endptr, base);

    if (*endptr != '\0')
    {
        success = false;
        return (0);
    }
    success = true;
    return (value);
}

void    RequestParser::handleMultiPart()
{
    if (!request_.getIsMultipart())
        return ;

    MultipartParser multipart_parser;
    try {
        MultipartData   data = multipart_parser.parse(
            request_.getBody(),
            request_.getHeaderValue(Http::Headers::CONTENT_TYPE)
        );
        request_.setMultipartData(data);
    } catch (const std::runtime_error& e)
    {
        // TODO: remove this log ?
        std::cerr << "Multipart parsing error: " << e.what() << std::endl;
        error_code_ = 400; // TODO: BAD_REQUEST
        state_ = ParserState::ERROR;
    }
}
