/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:50:43 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/02 22:12:32 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <vector>
#include <string>
#include <inttypes.h>   // uint16_t
#include "parser/ServerConfig.hpp"
#include "http/RequestParser.hpp"
#include "reactor/BaseEventHandler.hpp"
#include "reactor/IEventHandler.hpp"
#include "reactor/Reactor.hpp"
#include "routing/LocationMatcher.hpp"
#include "routing/ServerMatcher.hpp"

/*
Internally it:
    - Owns the socket fd
    - Handles EPOLLIN / EPOLLOUT
    - Reads request
    - Parses HTTP
    - Builds response
    - Changes its internal state

enum State {
    READING_HEADERS,
    READING_BODY,
    PROCESSING,
    WRITING_RESPONSE,
    CLOSED
};
*/

class ConnectionHandler : public BaseEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    ConnectionHandler(
        int client_fd,
        Reactor& reactor,
        uint16_t port,
        const std::vector<ServerConfig>& servers
    );
    ~ConnectionHandler(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    // ---------- Inherited Member Methods -----------------------
    int     getFd() const;
    void    handleRead();
    void    handleWrite();
    void    handleError();

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int                         fd_;
    Reactor&                    reactor_;
    RequestParser               request_parser_;
    size_t                      bytes_sent_;
    std::string                 serialized_response_;
    uint16_t                    port_;
    const                       std::vector<ServerConfig>& servers_;
    bool                        server_resolved_;
    ServerConfig                selected_server_;
    LocationConfig              selected_location_;
    std::string                 cgi_output_buffer_;
    bool                        cgi_pending_;

    void                        prepareResponse(size_t status);
    void                        resolveServerAndLocation();
    bool                        checkBodySizeLimit();
    bool                        checkIsMethodAllowed();
    void                        handleCgi();

    ConnectionHandler& operator=(const ConnectionHandler& copy);
    ConnectionHandler(const ConnectionHandler& copy);

};

#endif // CONNECTION_HANDLER_HPP
