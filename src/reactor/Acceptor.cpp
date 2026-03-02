/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Acceptor.cpp                                       :+:      :+:    :+:   */
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
#include "reactor/Acceptor.hpp"
#include "utils/Utils.hpp"

Acceptor::Acceptor(Socket *server_socket, Reactor& reactor)
    :   server_socket_(server_socket),
        reactor_(reactor)
{
}

Acceptor::~Acceptor(void)
{
}

int Acceptor::getFd() const
{
    return (server_socket_->getFd());
}

void Acceptor::handleWrite() {}

void Acceptor::handleError() {}

void Acceptor::handleRead()
{
    while (true)
    {
        int client_fd = server_socket_->accept();
        if (client_fd == -1)
            break ;
        Utils::setNonBlocking(client_fd);
        ConnectionHandler *connection_handler = new ConnectionHandler( client_fd, reactor_); // TODO: how to free memory if exception is throwing?
        reactor_.addHandler(connection_handler, EPOLLIN);
    }
}
