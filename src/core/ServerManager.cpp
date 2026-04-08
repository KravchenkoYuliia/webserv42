/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 18:51:35 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/08 15:34:48 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include "utils/Utils.hpp"
#include "reactor/ConnectionAcceptor.hpp"
#include "core/ServerManager.hpp"
#include <iostream>

ServerManager::ServerManager(void)
{
}

ServerManager::~ServerManager(void)
{
     for (std::vector<Socket*>::iterator it = listening_sockets_.begin(); it != listening_sockets_.end(); ++it)
    {
        delete *it;
    }
}

void ServerManager::init( const std::map<uint16_t , std::vector<ServerConfig> >& servers_by_port )
{
    typedef std::map<uint16_t, std::vector<ServerConfig> >::const_iterator it_type;
    for (it_type it = servers_by_port.begin(); it != servers_by_port.end(); ++it)
    {
        uint16_t port = it->first;
        Socket *socket = new Socket();
        try {
            socket->create();
            socket->setReusable();
            socket->bind(port);
            Utils::setNonBlocking(socket->getFd());
            socket->listen();
            listening_sockets_.push_back(socket);

        }
        catch (const std::exception& e)
        {
            delete socket;
            throw; // rethrow the exception after cleanup
        }

        ConnectionAcceptor *acceptor = new ConnectionAcceptor(socket, reactor_, port, it->second);
        try {
            reactor_.addHandler(acceptor);
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception caught while adding handler: " << e.what() << std::endl;
            delete acceptor;
            delete socket;
            throw; // rethrow the exception after cleanup
        }
    }
}

void    ServerManager::run()
{
    reactor_.run();
}
