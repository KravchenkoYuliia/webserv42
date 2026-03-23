/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 11:44:57 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/20 16:28:35 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_CONSTANTS_HPP
#define HTTP_CONSTANTS_HPP

#include <string>

struct Http
{
    struct Protocol {
        static const std::string HTTP_VERSION_1_0;
        static const std::string HTTP_VERSION_1_1;
    };

    struct Method {
        static const std::string GET;
        static const std::string POST;
        static const std::string DELETE;
        static const std::string UNKNOWN;
    };

    struct Headers {
        static const std::string HOST;
        static const std::string CONTENT_LENGTH;
        static const std::string TRANSFER_ENCODING;
        static const std::string CONTENT_TYPE;
        static const std::string CONNECTION;
    };

    struct Formatting {
        static const char   *CR;
        static const char   *LF;
        static const char   *CRLF;
        static const size_t CRLF_SIZE;
        static const char   *HEADER_END;
        static const size_t HEADER_END_SIZE;
        static const char   *SPACE;
        static const char   *COLON_SEPARATOR;
        static const char   *HEADER_SEPARATOR;
        static const char   *BOUNDARY_PREFIX;
        static const char   *BOUNDARY_SUFFIX;
        static const size_t BOUNDARY_DELIMITER_SIZE;
    };

    struct ContentType {
        static const std::string TEXT_PLAIN;
        static const std::string MULTIPART_FORM_DATA;
        static const std::string APPLICATION_X_WWW_FORM_URLENCODED;
    };

    struct MultiPart {
        static const std::string BOUNDARY;
        static const std::string NAME;
        static const std::string FILENAME;
        static const std::string CONTENT_DISPOSITION;
        static const std::string CONTENT_TYPE;
    };
};

#endif // HTTP_CONSTANTS_HPP
