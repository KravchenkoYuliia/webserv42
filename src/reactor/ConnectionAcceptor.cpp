/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionAcceptor.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 12:20:39 by jgossard          #+#    #+#             */
/*   Updated: 2026/02/27 19:05:06 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include <sys/epoll.h>
#include "core/ConnectionHandler.hpp"
#include "reactor/ConnectionAcceptor.hpp"
#include "utils/Utils.hpp"

ConnectionAcceptor::ConnectionAcceptor(
    Socket *server_socket,
    Reactor& reactor,
    uint16_t port,
    const std::vector<ServerConfig>& servers
):  BaseEventHandler(BaseEventHandler::ACCEPTOR),
    server_socket_(server_socket),
    reactor_(reactor),
    port_(port),
    servers_(servers)
{
}

ConnectionAcceptor::~ConnectionAcceptor(void)
{
}

int ConnectionAcceptor::getFd() const
{
    return (server_socket_->getFd());
}

void ConnectionAcceptor::handleWrite() {}

void ConnectionAcceptor::handleError() {}

void ConnectionAcceptor::handleRead()
{
    while (true)
    {
        int client_fd = server_socket_->accept();
        if (client_fd == -1)
            break ;
        Utils::setNonBlocking(client_fd);
        std::cout << "New client connected! client_fd = " << client_fd << std::endl;
        ConnectionHandler *connection_handler = new ConnectionHandler( client_fd, reactor_, port_, servers_ );
        try {
            reactor_.addHandler(connection_handler);
        } catch (const std::exception& e)
        {
            delete connection_handler;
            throw; // rethrow the exeception after cleanup
        }
    }
}
