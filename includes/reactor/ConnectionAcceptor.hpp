/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionAcceptor.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 10:15:54 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 12:45:02 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include <vector>
#include <inttypes.h>   // uint16_t
#include "core/Socket.hpp"
#include "parser/ServerConfig.hpp"
#include "reactor/BaseEventHandler.hpp"
#include "reactor/Reactor.hpp"

/*
    An Acceptor (Listening Socket Handler)
    This handles:
    - Accept new clients
    - Create new Connection objects
    - Register them into Reactor
    - Listens on a server socket
    - EPOLLIN on server socket
*/

class ConnectionAcceptor : public BaseEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    ConnectionAcceptor(
        Socket *socket,
        Reactor& reactor,
        uint16_t port,
        const std::vector<ServerConfig>& servers
    );
    ~ConnectionAcceptor(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    // ---------- Inherited Member Methods -----------------------
    virtual void handleRead();
    virtual void handleWrite();
    virtual void handleError();
    // virtual void handleEvent();
    virtual int  getFd() const;

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    Socket      *server_socket_;
    Reactor&    reactor_;
    uint16_t    port_;
    const       std::vector<ServerConfig>& servers_;

    ConnectionAcceptor(const ConnectionAcceptor& copy);
    ConnectionAcceptor& operator=(const ConnectionAcceptor& copy);

};

#endif // ACCEPTOR_HPP
