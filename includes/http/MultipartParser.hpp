/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MultipartParser.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 13:17:27 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/23 16:12:50 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MULTIPART_PARSER_HPP
#define MULTIPART_PARSER_HPP

#include <string>
#include <map>
#include "http/MultipartData.hpp"

class MultipartParser {
public:
    // ---------- Constructors / Destructor ----------

    MultipartParser(void);
    ~MultipartParser(void);

    // ---------- Overloading Operators Methods -------
    // ---------- Getter and Setter Methods ------------

    MultipartData&  getMultipartData();

    // ---------- Member Methods -----------------------
    MultipartData    parse( const std::string& body, const std::string& contentType );

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ----------------------- Private Data members -----------------------
    MultipartData   multipart_data_;



    // ---------- Private Method Members -------------------------

    std::string                         extractBoundary( const std::string& contentType );
    std::map<std::string, std::string>  parseHeaders( const std::string& headers );
    void                                parseParts( const std::string& body, const std::string& boundary );
    void                                parsePart( const std::string& body, size_t start_pos, size_t end_pos );
    void                                parsePartHeaders(
                                            const std::string& body,
                                            const std::string& part,
                                            size_t body_start,
                                            size_t body_end );

    std::string                         extractFilename( const std::string& raw_headers );
    std::string                         extractContentType( const std::string& raw_headers );
    std::string                         extractName( const std::string& raw_headers );
    std::string                         extractHeaderValue(
                                            const std::string& raw_headers,
                                            const std::string& header_key,
                                            const std::string& default_header_name
                                        );

    MultipartParser(const MultipartParser& copy);
    MultipartParser& operator=(const MultipartParser& copy);
};

#endif // MULTIPART_PARSER_HPP
