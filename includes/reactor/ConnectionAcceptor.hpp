/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionAcceptor.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 10:15:54 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/02 10:18:12 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "core/Socket.hpp"
#include "reactor/IEventHandler.hpp"
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

class ConnectionAcceptor : public IEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    ConnectionAcceptor(Socket *socket, Reactor& reactor);
    ~ConnectionAcceptor(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------
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

    ConnectionAcceptor(const ConnectionAcceptor& copy);
    ConnectionAcceptor& operator=(const ConnectionAcceptor& copy);

};

#endif // ACCEPTOR_HPP
