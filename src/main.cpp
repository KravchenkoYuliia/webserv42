/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 12:08:39 by yukravch          #+#    #+#             */
/*   Updated: 2026/03/05 10:18:01 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TODO: remove the following lines
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h> // htons , htonl
#include <cstring> //for memset
#include <unistd.h> // for close
#include <errno.h> // errno
// end here

#include <exception>
#include "ConfigParser.hpp"
#include "core/Socket.hpp"
#include "utils/Utils.hpp"
#include "core/ServerManager.hpp"

int main(int argc, char **argv)
{

    if ( argc != 2 )
    {
        std::cerr << "Usage: ./webserv <config_file> " << std::endl;
        return 1;
    }

    ServerManager server_manager;

    if ( argc != 2 )
    {
        std::cerr << "Usage: ./webserv <config_file> " << std::endl;
        return 1;
    }

    try {
        (void)argv; // TODO: delete line
        ConfigParser configParser( argv[1] );
        /*server_manager.init(8080);
        server_manager.run();*/
    } catch (const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}

// int main(int argc, char **argv)
// {
// 	(void)argc; // TODO: delete line
// 	(void)argv; // TDOD: delete line

// 	// Create main socket
// 	int	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
// 	if ( socket_fd == -1 )
// 		return 1;

// 	// Make socket reusable
// 	int opt = 1;
// 	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
// 	{
// 		close(socket_fd);
// 		return 1;
// 	}

// 	// BIND preparation
// 	struct sockaddr_in	address;
// 	std::memset( &address, 0, sizeof(address) );
// 	address.sin_family = AF_INET;
// 	address.sin_port = htons(8080);
// 	address.sin_addr.s_addr = INADDR_ANY;


// 	int	bind_status = bind( socket_fd, (struct sockaddr*)&address, sizeof(address) );
// 	if ( bind_status == -1 ) {

// 		std::cerr << "bind failed" << std::endl;
// 		close(socket_fd);
// 		return 1;
// 	}

// 	// Set socket non-blocking
// 	int	flag = fcntl( socket_fd, F_GETFL );
// 	if ( flag == -1 ) {
// 		std::cerr << " socket fcntl SET failed" << std::endl;
// 		close(socket_fd);
// 		return 1;
// 	}
// 	if ( fcntl( socket_fd, F_SETFL, flag | O_NONBLOCK ) == -1 ) {
// 		std::cerr << " socket fcntl GET failed" << std::endl;
// 		close(socket_fd);
// 		return 1;
// 	}

// 	// Listening socket
// 	int	listen_status = listen( socket_fd, SOMAXCONN);
// 	if ( listen_status == -1 )
// 	{
// 		close(socket_fd);
// 		return 1;
// 	}

// 	// Create EPOLL
// 	int	epoll_fd = epoll_create(1); //create an instance of epoll
// 	if ( epoll_fd == -1 ) {

// 		std::cerr << "epoll_create failed" << std::endl;
// 		close(socket_fd);
// 		return 1;
// 	}

// 	struct	epoll_event	event;
// 	event.events = EPOLLIN; // tells when data is available to read (readable) instead of checking everything constantly
// 	event.data.fd = socket_fd;
// 	if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, socket_fd, &event ) == -1 ) { //register event

// 		std::cerr << "epoll_ctl for socket_fd failed" << std::endl;
// 		close(socket_fd);
// 		close(epoll_fd);
// 		return 1;
// 	}

// 	int kMaxReadyEventsBatchSize = 1024;
// 	struct epoll_event ready_events_list[kMaxReadyEventsBatchSize];

// 	// === Per-client state (single client for simplicity) ===
// 	int new_client_accepted = -1;
// 	// char request_buffer[8192];
// 	// int request_size = 0;
// 	std::string response =
// 		"HTTP/1.1 200 OK\r\n"
// 		"Content-Length: 24\r\n"
// 		"Connection: close\r\n"
// 		"\r\n"
// 		"Hello pink t-shirts team";
// 	int response_length = response.length();
// 	int bytes_sent = 0;

// 	// Start listening
// 	while (true)
// 	{
// 		int num_fds_ready = epoll_wait(epoll_fd, ready_events_list, kMaxReadyEventsBatchSize, -1);
// 		if (num_fds_ready == -1)
// 		{
// 			if (errno == EINTR)
// 				continue;
// 			std::cerr << "epoll_wait failed" << std::endl;
// 			close(socket_fd);
// 			if (new_client_accepted >= 0)
// 				close(new_client_accepted);
// 			close(epoll_fd);
// 			return (1);
// 		}

// 		// Iterate over the list of ready_events_list
// 		for (int i = 0; i < num_fds_ready; ++i)
// 		{
// 			int			ready_client_fd = ready_events_list[i].data.fd;
// 			uint32_t	ready_events = ready_events_list[i].events;

// 			if (ready_client_fd == socket_fd)
// 			{
// 				// while (true)
// 				// {
// 					new_client_accepted = accept(socket_fd, NULL, NULL);
// 					if (new_client_accepted == -1)
// 					{
// 						// the following is usefull because connetcion are closed automatically and it is not an error
// 						if (errno == EAGAIN || errno == EWOULDBLOCK)
// 						{
// 							close(ready_client_fd);
// 							break;
// 						}
// 						else
// 						{
// 							std::cerr << "client accept failed";
// 							close(ready_client_fd);
// 							break;
// 						}
// 					}

// 					//make non blocking;
// 					int	flag = fcntl( new_client_accepted, F_GETFL );
// 					if ( flag == -1 ) {
// 						std::cerr << " fcntl SET failed" << std::endl;
// 						close(new_client_accepted);
// 						close(ready_client_fd);
// 						break;
// 					}
// 					if ( fcntl( new_client_accepted, F_SETFL, flag | O_NONBLOCK ) == -1 ) {
// 						std::cerr << " fcntl GET failed" << std::endl;
// 						close(new_client_accepted);
// 						close(ready_client_fd);
// 						break;
// 					}

// 					struct	epoll_event	event;
// 					event.events = EPOLLIN;
// 					event.data.fd = new_client_accepted;
// 					if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, new_client_accepted, &event) == -1 ) {
// 						std::cerr << " epoll_ctl for new_client_accepted failed" << std::endl;
// 						close(new_client_accepted);
// 						close(ready_client_fd);
// 						return 1;
// 					}
// 						std::cout << "New client connected: " << new_client_accepted << std::endl;
// 				// }
// 			}
// 			else
// 			{
// 				// EPOLLIN
// 				if (ready_events & EPOLLIN)
// 				{
// 					char buffer[8192];
// 					std::cout << "EPOLLIN case" << std::endl;
// 					ssize_t bytes = recv(ready_client_fd, buffer, sizeof(buffer), 0);
// 					if (bytes <= 0)
// 					{
// 						// TODO: remove check with EAGAIN
// 						if (errno == EAGAIN || errno == EWOULDBLOCK)
// 							break;
// 						std::cout << ": Client disconnected: " << ready_client_fd << std::endl;
// 						close(ready_client_fd);
// 						break;
// 					}
// 					// if (request_size >= 4 && strstr(request_buffer, "\r\n\r\n"))
// 					// {
// 						// Switch to write mode
// 						struct epoll_event ev;
// 						ev.events = EPOLLOUT;  // Switch socket to write mode
// 						ev.data.fd = ready_client_fd;

// 						epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ready_client_fd, &ev);
// 						std::cout << "SWITCH socket to EPOLLOUT" << std::endl;
// 					// 	break;
// 					// }
// 				}

// 				// EPOLLOUT
// 				if (ready_events & EPOLLOUT)
// 				{
// 					std::cout << "EPOLLOUT case" << std::endl;

// 					while (bytes_sent < response_length) {
// 						ssize_t bytes = send(
// 							ready_client_fd,
// 							response.c_str(),
// 							response_length,
// 							0
// 						);

// 						if (bytes == -1) {
// 							std::cerr << "Error sending data!" << std::endl;
// 							break;
// 						}

// 						bytes_sent += bytes;

// 						std::cout << "Sent " << bytes << " bytes, total sent: "
// 								<< bytes_sent << " / " << response_length << std::endl;

// 						// Optional: print the actual chunk sent (for debugging)
// 						std::string chunk = response.substr(bytes_sent - bytes, bytes);
// 						std::cout << "Chunk sent:\n" << chunk << std::endl << "-----" << std::endl;
// 					}

// 					// Done sending → close connection
// 					if (bytes_sent >= response_length)
// 					{
// 						close(ready_client_fd);
// 						bytes_sent = 0;
// 					}

// 				}
// 			}
// 		}

// 	}
// 	close(socket_fd);
// 	return (0);
// }
