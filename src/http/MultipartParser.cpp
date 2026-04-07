/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultipartParser.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:58:31 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 18:38:13 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fstream>  // ofstream
#include <stdexcept>        //std::runtime_error
#include "http/HttpConstants.hpp"
#include "http/MultipartParser.hpp"

// ############################ MultipartParser Class ############################

// ------------------------- Destructor / Constructor -------------------------

MultipartParser::MultipartParser(void) : multipart_data_()
{
}

MultipartParser::~MultipartParser(void)
{
}

// --------------------------- Public Getter Method ---------------------------

MultipartData&  MultipartParser::getMultipartData()
{
    return (multipart_data_);
}

// --------------------------- Public member Method ---------------------------

/**
 * @brief Parses a multipart/form-data body and returns the parsed data.
 *
 * This method processes a given body and Content-Type header. It first validates the content type, ensuring
 * that it is of type 'multipart/form-data' and contains a valid boundary. If the conditions are met, it
 * extracts the boundary and parses the parts of the multipart data.
 *
 * @param body The body of the HTTP request or response containing multipart data.
 * @param contentType The content type of the request, which should include 'multipart/form-data' and a boundary.
 *
 * @throws std::runtime_error If the content type is not 'multipart/form-data' or if the boundary is missing or malformed.
 *
 * @return MultipartData A structure containing the parsed multipart data.
 */
MultipartData    MultipartParser::parse( const std::string& body, const std::string& contentType )
{
    if (contentType.find(Http::ContentType::MULTIPART_FORM_DATA) == std::string::npos)
        throw std::runtime_error("Invalid content-type. Expected 'multipart/form-data', but got: " + contentType);
    if (contentType.find(Http::MultiPart::BOUNDARY) == std::string::npos)
        throw std::runtime_error("Multipart content-type is missing a boundary. Ensure that the 'boundary' parameter is included in the Content-Type header. Found content-type: " + contentType);
    parseParts( body, extractBoundary( contentType ) );
    return (multipart_data_);
}

// --------------------------- Private member Method ---------------------------

/**
 * @brief Extracts the boundary from the Content-Type header.
 *
 * This method searches the Content-Type header for the boundary parameter and extracts its value. It also checks
 * that the boundary is properly formatted and non-empty.
 *
 * @param contentType The content type of the request, which must contain a boundary parameter.
 *
 * @throws std::runtime_error If the boundary is missing, empty, or malformed.
 *
 * @return std::string The extracted boundary prefixed with the necessary boundary marker.
 */
std::string     MultipartParser::extractBoundary( const std::string& contentType )
{
    const std::string   delimiter = Http::MultiPart::BOUNDARY;
    size_t              position = contentType.find(delimiter);

    if (position == std::string::npos)
        throw std::runtime_error("Failed to extract boundary from content-type. Boundary is required for multipart parsing.");
    std::string boundary = contentType.substr( position + delimiter.size() );
    if (boundary.empty() || boundary == "\"\"")
        throw std::runtime_error("Boundary is empty or malformed in the Content-Type header. Boundary is required for multipart parsing.");
    return ( Http::Formatting::BOUNDARY_PREFIX + boundary );
}

/**
 * @brief Parses the multipart body and extracts individual parts.
 *
 * This method splits the multipart body into its individual parts by searching for the boundary markers.
 * It processes each part by finding the start and end positions and then delegates the parsing of each part
 * to the `parsePart` method.
 *
 * @param body The body of the HTTP request containing the multipart data.
 * @param boundary The boundary string used to separate different parts of the multipart data.
 */
void    MultipartParser::parseParts( const std::string& body, const std::string& boundary )
{
    size_t  pos = 0;

    while (pos < body.size())
    {
        size_t start_pos = body.find(boundary, pos);
        if (start_pos == std::string::npos)
            break ;
        start_pos += boundary.size();
        if (body.size() >= start_pos + Http::Formatting::BOUNDARY_DELIMITER_SIZE && body.substr( start_pos, Http::Formatting::BOUNDARY_DELIMITER_SIZE ) == Http::Formatting::BOUNDARY_SUFFIX)
            break;
        if (body.size() >= start_pos + Http::Formatting::CRLF_SIZE && body.substr( start_pos, Http::Formatting::CRLF_SIZE ) == Http::Formatting::CRLF)
            start_pos += Http::Formatting::CRLF_SIZE;
        size_t end_pos = body.find(boundary, start_pos);
        if (end_pos == std::string::npos)
            break;
        parsePart( body, start_pos, end_pos );
        pos = end_pos;
    }
}

/**
 * @brief Parses an individual part of the multipart body.
 *
 * This method processes a single part of the multipart data. It extracts headers and body content, and then
 * delegates the parsing of headers to the `parsePartHeaders` method.
 *
 * @param body The body of the HTTP request or response containing the multipart data.
 * @param start_pos The start position of the part within the body.
 * @param end_pos The end position of the part within the body.
 */
void    MultipartParser::parsePart( const std::string& body, size_t start_pos, size_t end_pos )
{
    size_t  header_end_pos = body.find( Http::Formatting::HEADER_END, start_pos );
    if (header_end_pos == std::string::npos || header_end_pos >= end_pos)
        return ;

    std::string raw_headers = body.substr( start_pos, header_end_pos - start_pos );
    size_t      body_start = header_end_pos + Http::Formatting::HEADER_END_SIZE;
    size_t      body_end = end_pos;

    if (body_end >= body_start + Http::Formatting::CRLF_SIZE
            && body.substr(body_end - Http::Formatting::CRLF_SIZE, Http::Formatting::CRLF_SIZE) == Http::Formatting::CRLF)
    {
        body_end -= Http::Formatting::CRLF_SIZE;
    }
    parsePartHeaders( body, raw_headers, body_start, body_end );
}

/**
 * @brief Parses the headers of a multipart part and processes its content.
 *
 * This function checks for the presence of the `Content-Disposition` header and validates that it contains
 * the `name` parameter. If a `filename` is present in the headers, it extracts the `filename` and `Content-Type`.
 * Based on the presence of the `filename`, it either stores the part as a file or as a field.
 *
 * If the `Content-Disposition` header or the `name` parameter is missing, an exception is thrown.
 *
 * @param body The body content of the multipart request.
 * @param raw_headers The raw headers of the multipart part.
 * @param body_start The starting index of the body content for this part.
 * @param body_end The ending index of the body content for this part.
 *
 * @throws std::runtime_error if the `Content-Disposition` header or `name` parameter is missing.
 */
void    MultipartParser::parsePartHeaders( const std::string& body, const std::string& raw_headers, size_t body_start, size_t body_end )
{
    if (raw_headers.find("Content-Disposition:") == std::string::npos)
        throw std::runtime_error("Missing Content-Disposition header");
    if (raw_headers.find("name=\"") == std::string::npos)
        throw std::runtime_error("Missing 'name' parameter in Content-Disposition");

    const std::string   name = extractName( raw_headers );
    std::string         filename;
    std::string         content_type;

    if (raw_headers.find(Http::MultiPart::FILENAME) != std::string::npos)
    {
        filename = extractFilename( raw_headers );
        content_type = extractContentType( raw_headers );
        UploadedFile    file(filename, content_type, body.substr(body_start, body_end - body_start));
        multipart_data_.addFile(name, file);
    }
    else
    {
        multipart_data_.addField(name, body.substr(body_start, body_end - body_start));
    }
}

std::string  MultipartParser::extractFilename( const std::string& raw_headers )
{
    const std::string   default_filename = "default_uploaded_filename";
    const std::string   filename_header = "filename=\"";
    return ( extractHeaderValue( raw_headers, filename_header, default_filename ) );
}

std::string  MultipartParser::extractContentType( const std::string& raw_headers )
{
    const std::string   default_content_type = "default_content_type";
    const std::string   content_type_header = "Content-Type=\"";
    return ( extractHeaderValue( raw_headers, content_type_header, default_content_type ) );
}

std::string    MultipartParser::extractName( const std::string& raw_headers )
{
    const std::string   default_name_field = "default_name_field";
    const std::string   name_header = "name=\"";
    return ( extractHeaderValue( raw_headers, name_header, default_name_field ) );
}

std::string  MultipartParser::extractHeaderValue(
    const std::string& raw_headers,
    const std::string& header_key,
    const std::string& default_header_name
){
    std::string     name_field = default_header_name;
    size_t          start_pos = raw_headers.find(header_key);
    if (start_pos != std::string::npos)
    {
        start_pos += header_key.size();
        size_t end_pos = raw_headers.find("\"", start_pos);
        if (end_pos != std::string::npos && end_pos > start_pos)
            name_field = raw_headers.substr(start_pos, end_pos - start_pos);
    }
    return (name_field);
}
