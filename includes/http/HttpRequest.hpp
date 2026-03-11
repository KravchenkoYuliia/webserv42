/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 17:50:45 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:03:31 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <map>

class HttpRequest {
public:
    enum Method { GET, POST, DELETE, UNKNOWN };

    // ---------- Constructors / Destructor ----------

    HttpRequest(void);
    ~HttpRequest(void);

    // ---------- Overloading Operators Methods -------



    // ---------- Getter and Setter Methods ------------
    void                                            setMethod(Method method);
    void                                            setMethod(const std::string& method);
    void                                            setUri(const std::string& uri);
    void                                            setVersion(const std::string& version);
    void                                            setHeader(const std::string& key, const std::string& value);
    void                                            setBody(const std::string& body);
    void                                            setContentLength(size_t value);
    void                                            setChunkSize(size_t value);

    Method                                          getMethod() const;
    std::string                                     getMethodToString() const;
    const std::string&                              getUri() const;
    const std::string&                              getVersion() const;
    const std::map<std::string, std::string>&       getHeaders() const;
    std::string                                     getHeader(const std::string& key) const;
    const std::string&                              getBody() const;
    size_t                                          getContentLength() const;
    size_t                                          getChunkSize() const;

    // ---------- Member Methods -----------------------
    bool                                            hasHeader( const std::string& key) const;

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    Method                                  method_;
    std::string                             uri_;
    std::string                             version_;
    std::map<std::string, std::string>      headers_;
    std::string                             body_;
    size_t                                  content_length_;
    size_t                                  chunk_size_;

    HttpRequest& operator=(const HttpRequest& copy);
    HttpRequest(const HttpRequest& copy);

};

#endif // HTTP_REQUEST_HPP
