/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:50:43 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 15:19:41 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HANDLER_HPP
#define CONNECTION_HANDLER_HPP

#include <string>
#include "http/RequestParser.hpp"
#include "reactor/Reactor.hpp"
#include "reactor/IEventHandler.hpp"


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

class ConnectionHandler : public IEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    ConnectionHandler(int client_fd, Reactor& reactor);
    ~ConnectionHandler(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------
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
    ConnectionState             state_;
    int                         bytes_sent_;
    std::string                 serialized_response_;


    ConnectionHandler& operator=(const ConnectionHandler& copy);
    ConnectionHandler(const ConnectionHandler& copy);

};

#endif // CONNECTION_HANDLER_HPP
