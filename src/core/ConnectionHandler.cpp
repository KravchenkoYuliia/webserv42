/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:42:02 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/02 12:42:27 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>         // close
#include <sys/epoll.h>      // EPOLLIN , EPOLLOUT
#include <sys/socket.h>     // recv
#include <errno.h>          // errno, EAGAIN, EWOULDBLOCK
#include "core/ConnectionHandler.hpp"
#include "utils/Utils.hpp"

ConnectionHandler::ConnectionHandler(int client_fd, Reactor& reactor)
    :   fd_(client_fd),
        reactor_(reactor)
{
    std::cout << "ConnectionHandler default constructor called" << std::endl;
}

ConnectionHandler::~ConnectionHandler(void)
{
    if (fd_ != -1)
        ::close(fd_);
}

int ConnectionHandler::getFd() const
{
    return (fd_);
}

void ConnectionHandler::handleRead()
{
    char buffer[8192];
    std::cout << "EPOLLIN case" << std::endl;
    ssize_t bytes = recv(fd_, buffer, sizeof(buffer), 0);
    if (bytes <= 0)
    {
        // TODO: remove check with EAGAIN
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return ;
        reactor_.deleteHandler(fd_);
        return ;
    }
    // Switch to write mode
    reactor_.updateHandler(this, EPOLLOUT); // TODO: not sure about the "this"
}

// TODO: add check on bytes_send
// TODO: track progress across multiple EPOLLOUT events.
void ConnectionHandler::handleWrite()
{
    std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Length: 24\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello pink t-shirts team";
	int response_length = response.length();
	int bytes_sent = 0;
    while (bytes_sent < response_length)
    {
        ssize_t bytes = send(fd_, response.c_str(), response_length, 0 );
        if (bytes == -1)
        {
            std::cerr << "Error sending data!" << std::endl;
            break;
        }

        bytes_sent += bytes;

        std::cout << "Sent " << bytes << " bytes, total sent: "
                << bytes_sent << " / " << response_length << std::endl;

        // Optional: print the actual chunk sent (for debugging)
        std::string chunk = response.substr(bytes_sent - bytes, bytes);
        std::cout << "Chunk sent:\n" << chunk << std::endl << "-----" << std::endl;
    }

    // Done sending → close connection
    if (bytes_sent >= response_length)
    {
        bytes_sent = 0;
    }
    // finish to send , so remove the fd from reading_fd_list_
    reactor_.deleteHandler(fd_);
}

void ConnectionHandler::handleError()
{
    reactor_.deleteHandler(fd_);

}
