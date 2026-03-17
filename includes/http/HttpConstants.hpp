/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpConstants.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 11:44:57 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/17 11:07:02 by jgossard         ###   ########.fr       */
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
        static const char *CR;
        static const char *LF;
        static const char *CRLF;
        static const char *HEADER_END;
        static const char *SPACE;
        static const char *COLON_SEPARATOR;
        static const char *HEADER_SEPARATOR;
    };

    struct ContentType {
        static const std::string TEXT_PLAIN;
        static const std::string MULTIPART_FORM_DATA;
    };
};

#endif // HTTP_CONSTANTS_HPP
