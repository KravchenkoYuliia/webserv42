/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:28:58 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/26 11:57:09 by jgossard         ###   ########.fr       */
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
    const std::string&      getHeader( const std::string& key ) const;
    const std::string&      getUri() const;
    int                     getErrorCode() const;

    // ---------- Member Methods -----------------------
    void            appendData( const char *data, size_t len );
    bool            parseRequest( const std::string& ); // or parseRequestBytes or process
    ResultType      parseNext(); // or parseRequestBytes or process
    bool            isComplete() const;

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ----------------------- Private Data members -----------------------
    HttpRequest             request_;
    std::string             raw_buffer_; // store the accumulated bytes receive by recv
    ParserState::Type       state_;
    int                     error_code_;
    size_t                  content_length_bytes_;
    size_t                  current_chunk_size_;
    bool                    waiting_for_chunk_size_;


    // ---------------------- Private Member Methods ----------------------
    ResultType            parseRequestLine();
    ResultType            parseHeaders();
    ResultType            parseBodyContentLength();
    ResultType            parseBodyChunked();

    // ------------------------- Private helpers -------------------------

    bool            hasEndOfLine() const;
    std::string     extract_header_line();
    bool            parseRequestLineFields( const std::string& line );
    bool            parseHeaderLine( const std::string& line );
    bool            validateHeaderSet();
    bool            validateHostHeader();
    bool            validateHeaderConflicts();
    bool            validateBodyHeaders();
    bool            validateTransferEncodingHeader();
    bool            validateContentLengthHeader();
    bool            validateBodyForMethod();
    bool            validateContentTypeHeader();
    void            handleMultiPart();

    // Utility
    size_t          findCRLF() const;
    bool            isValidMethod( const std::string& method );
    bool            isValidUriFormat( const std::string& uri );
    bool            isValidHttpProtocolVersion( const std::string& protocol_version );
    unsigned long   parseUnsignedLong( const std::string &str, bool &success, int base );
};

#endif // REQUEST_PARSER_HPP
