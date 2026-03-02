/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 18:51:35 by jgossard          #+#    #+#             */
/*   Updated: 2026/02/27 19:05:51 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/epoll.h>
#include "utils/Utils.hpp"
#include "reactor/Acceptor.hpp"
#include "core/ServerManager.hpp"
#include <iostream>

ServerManager::ServerManager(void)
{
    std::cout << "ServerManager default constructor called" << std::endl;
}

ServerManager::ServerManager(const ServerManager& copy)
{
    std::cout << "ServerManager copy constructor called" << std::endl;
    *this = copy;
}

ServerManager::~ServerManager(void)
{
    std::cout << "ServerManager destructor called" << std::endl;
}

ServerManager& ServerManager::operator=(const ServerManager& copy)
{
    std::cout << "ServerManager assignment operator called" << std::endl;
    if (this != &copy) {
        // copy data members here
    }
    return (*this);
}

void ServerManager::init( uint16_t port)
{
    server_socket_.create();
    server_socket_.setReusable();
    server_socket_.bind(port);
    Utils::setNonBlocking(server_socket_.getFd());
    server_socket_.listen();

    Acceptor *acceptor = new Acceptor(&server_socket_, reactor_);  // TODO: how to free memory if exception is throwing?
    reactor_.addHandler(acceptor, EPOLLIN);
}

void    ServerManager::run()
{
    reactor_.run();
}
