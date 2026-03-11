/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:47:03 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 15:42:19 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
public:
    // ---------- Constructors / Destructor ----------

    HttpResponse(void);
    HttpResponse(const HttpResponse& copy);
    ~HttpResponse(void);

    // ---------- Overloading Operators Methods -------

    HttpResponse& operator=(const HttpResponse& copy);

    // ---------- Getter and Setter Methods ------------

    void            setStatusCode(int code, const std::string& description);
    void            setHeader(const std::string& key, const std::string& value);
    void            setBody(const std::string& body);

    // ---------- Member Methods -----------------------

    std::string     serialize();

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int                                 status_code_value_;         // ex. 200
    std::string                         status_code_description_;   // ex. OK
    std::map<std::string, std::string>  headers_;                   // Host: webserv.fr
    std::string                         body_;
};

#endif // HTTP_RESPONSE_HPP
