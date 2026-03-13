/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:28:58 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/16 10:36:37 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include "http/HttpRequest.hpp"
#include "http/HttpResponse.hpp"

class RequestParser {
public:
    struct ParserResult {
        enum Type {
            OK,
            AGAIN,
            ERROR,
        };
    };
    struct ParserState {
        enum Type  {
            REQUEST_LINE,
            HEADERS,
            BODY_CONTENT_LENGTH,
            BODY_CHUNKED,
            BODY_NONE,
            COMPLETE,
            ERROR
        };
        static std::string toString(Type state);
    };

    typedef ParserResult::Type ResultType;

    // ---------- Constructors / Destructor ----------

    RequestParser(void);
    RequestParser(const RequestParser& copy);
    ~RequestParser(void);

    // ---------- Overloading Operators Methods -------

    RequestParser& operator=(const RequestParser& copy);

    // ---------- Getter and Setter Methods ------------

    const HttpRequest&      getRequest() const;
    std::string             getHeader( const std::string& key ) const;
    const std::string&      getUri() const;

    // ---------- Member Methods -----------------------
    void            appendData( const char *data, size_t len );
    bool            parseRequest( const std::string& ); // or parseRequestBytes or process
    ResultType      parseNext(); // or parseRequestBytes or process
    bool            isComplete() const;
    // ngx_http_parse_request_line
    // ngx_http_parse_header_line
    // ngx_http_parse_uri
    // ngx_http_parse_status_line
    // ngx_http_parse_multi_header_lines
    // ngx_http_arg
    // ngx_http_split_args
    // ngx_http_parse_chunked
protected:
    // ---------- Protected Data Members ---------------------

private:
    // ----------------------- Private Data members -----------------------
    HttpRequest             request_;
    HttpResponse            response_;
    std::string             raw_buffer_; // store the accumulated bytes receive by recv
    ParserState::Type       state_;
    int                     error_code_;

    // ---------------------- Private Member Methods ----------------------
    ResultType            parseRequestLine();
    ResultType            parseHeaders();
    ResultType            parseBodyContentLength();
    ResultType            parseBodyChunked();

    // ------------------------- Private helpers -------------------------

    bool            hasEndOfLine() const;
    std::string     extract_line();
    bool            parseRequestLineFields(const std::string& line);
    bool            parseHeaderLine(const std::string& line);
    bool            validateHeaderSet();

    // Utility
    size_t          findCRLF() const;
    bool            isValidMethod( const std::string& method );
    bool            isValidUriFormat( const std::string& uri );
    bool            isValidHttpProtocolVersion( const std::string& protocol_version );

};

#endif // REQUEST_PARSER_HPP
