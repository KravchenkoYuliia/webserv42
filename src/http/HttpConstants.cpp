/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 11:46:46 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/01 14:46:20 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpConstants.hpp"

// ------------------------- Struct Protocol -------------------------

const std::string Http::Protocol::HTTP_VERSION_1_0 = "HTTP/1.0";
const std::string Http::Protocol::HTTP_VERSION_1_1 = "HTTP/1.1";

// ------------------------- Struct Method -------------------------

const std::string Http::Method::GET = "GET";
const std::string Http::Method::POST = "POST";
const std::string Http::Method::DELETE = "DELETE";
const std::string Http::Method::UNKNOWN = "UNKNOWN";

// ------------------------- Struct Headers -------------------------

const std::string Http::Headers::HOST = "Host";
const std::string Http::Headers::CONTENT_LENGTH = "Content-Length";
const std::string Http::Headers::TRANSFER_ENCODING = "Transfer-Encoding";
const std::string Http::Headers::CONTENT_TYPE = "Content-Type";
const std::string Http::Headers::CONNECTION = "Connection";
const std::string Http::Headers::COOKIE = "Cookie";

// ------------------------- Struct Formatting -------------------------

const char      *Http::Formatting::CR = "\r";
const char      *Http::Formatting::LF = "\n";
const char      *Http::Formatting::CRLF = "\r\n";
const size_t     Http::Formatting::CRLF_SIZE = 2;
const char      *Http::Formatting::HEADER_END = "\r\n\r\n";
const size_t     Http::Formatting::HEADER_END_SIZE = 4;
const char      *Http::Formatting::SPACE = " ";
const char      *Http::Formatting::COLON_SEPARATOR = ":";
const char      *Http::Formatting::HEADER_SEPARATOR = ": ";
const char      *Http::Formatting::BOUNDARY_PREFIX = "--";
const char      *Http::Formatting::BOUNDARY_SUFFIX = "--";
const size_t     Http::Formatting::BOUNDARY_DELIMITER_SIZE = 2;


// ------------------------- Struct ContentType -------------------------

const std::string Http::ContentType::TEXT_PLAIN = "text/plain";
const std::string Http::ContentType::TEXT_HTML = "text/html";
const std::string Http::ContentType::MULTIPART_FORM_DATA = "multipart/form-data";
const std::string Http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED = "application/x-www-form-urlencoded";

// ------------------------- Struct Multipart -------------------------

const std::string Http::MultiPart::BOUNDARY = "boundary=";
const std::string Http::MultiPart::NAME = "name=";
const std::string Http::MultiPart::FILENAME = "filename=";
const std::string Http::MultiPart::CONTENT_DISPOSITION = "Content-Disposition:";
const std::string Http::MultiPart::CONTENT_TYPE = "Content-Type:";


const size_t      Http::Response::REQUEST_VALID = 1;
