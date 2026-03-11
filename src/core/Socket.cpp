/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 16:54:38 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/06 16:59:45 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>          // fcntl
#include <netinet/in.h>     // sockaddr_in
#include <arpa/inet.h>      // htons , htonl, uint16_t
#include <cstring>          // memset
#include <unistd.h>         // close
#include <errno.h>
#include <stdexcept>        //std::runtime_error
#include "core/Socket.hpp"

Socket::Socket(void)
    :   fd_(kDefaultFd),
        bind_port_(kDefaultPort)
{
    std::cout << "Socket parametized constructor called" << std::endl;
}

Socket::Socket( const Socket& copy )
{
    *this = copy;
}

Socket::~Socket(void)
{
    if (fd_ != -1)
        ::close(fd_);
}

Socket& Socket::operator=(const Socket& copy)
{
    if (this != &copy) {
        fd_ = copy.fd_; // TODO: issue with 2 potential fd pointing to the same address => fix this by using dup
        bind_port_ = copy.bind_port_;
    }
    return (*this);
}

int Socket::getFd() const
{
    return (fd_);
}

void Socket::create()
{
    int fd = ::socket( AF_INET, SOCK_STREAM, 0 );
    if (fd < 0)
        throw std::runtime_error("Socket creation failed");
    fd_ = fd;
}

void Socket::bind( uint16_t port )
{
    bind_port_  = port;

    struct sockaddr_in address;

    std::memset( &address, 0, sizeof(address) );
    address.sin_family = AF_INET;
    address.sin_port = htons(bind_port_);
    address.sin_addr.s_addr = INADDR_ANY;

    int bind_status = ::bind(fd_, (struct sockaddr*)&address, sizeof(address));
    if (bind_status < 0)
        throw std::runtime_error(std::string("bind failed! ") + std::strerror(errno));
}

void Socket::listen()
{
    int	listen_status = ::listen( fd_, SOMAXCONN);
	if ( listen_status == -1 )
        throw std::runtime_error(std::string("listen failed! ") + std::strerror(errno));
}

int Socket::accept()
{
    int new_fd = ::accept(fd_, NULL, NULL);
    if (new_fd < 0)
    {
        // TODO: remove this check?
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return (-1);
        throw std::runtime_error(std::string("accept failed! ") + std::strerror(errno));
    }
    return (new_fd);
}

void Socket::setReusable()
{
    int opt = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error(std::string("setsockopt failed! ") + std::strerror(errno));
}
