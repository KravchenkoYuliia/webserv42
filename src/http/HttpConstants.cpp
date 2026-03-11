/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 11:46:46 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 16:18:54 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HttpConstants.hpp"

// ------------------------- Struct Protocol -------------------------

const std::string Http::Protocol::HTTP_VERSION_1_0 = "HTTP/1.0";
const std::string Http::Protocol::HTTP_VERSION_1_1 = "HTTP/1.1";

// ------------------------- Struct Headers -------------------------

const std::string Http::Headers::HOST = "Host";
const std::string Http::Headers::CONTENT_LENGTH = "Content-Length";
const std::string Http::Headers::TRANSFER_ENCODING = "Transfer-Encoding";
const std::string Http::Headers::CONTENT_TYPE = "Content-Type";
const std::string Http::Headers::CONNECTION = "Connection";

// ------------------------- Struct Formatting -------------------------

const char *Http::Formatting::CR = "\r";
const char *Http::Formatting::LF = "\n";
const char *Http::Formatting::CRLF = "\r\n";
const char *Http::Formatting::HEADER_END = "\r\n\r\n";
const char *Http::Formatting::SPACE = " ";
const char *Http::Formatting::COLON_SEPARATOR = ":";
const char *Http::Formatting::HEADER_SEPARATOR = ": ";
