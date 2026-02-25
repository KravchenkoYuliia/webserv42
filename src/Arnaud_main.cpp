/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 15:12:11 by yukravch          #+#    #+#             */
/*   Updated: 2026/02/24 11:48:54 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> // fcntl
#include <arpa/inet.h>
#include <cstring> //for memset
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h> // epoll_creat, epoll_ctl, epoll_wait
#include <errno.h> // errno
#include <string.h>

int safe_close(int *fd)
{
	if (!fd || *fd < 0)
		return (0);
	int exit_code = -1;

	while (true)
	{
		exit_code = close(*fd);
		if (exit_code != -1 || errno != EINTR)
			break;
	}

	if (exit_code == -1)
	{
		std::cerr << "close failed" << std::endl;
		return (exit_code);
	}
	*fd = -1;
	return (0);
}

int	main(){

	int	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( socket_fd == -1 )
		return 1;

	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		safe_close(&socket_fd);
		return 1;
	}

	struct sockaddr_in	address;
	std::memset( &address, 0, sizeof(address) );
	address.sin_family = AF_INET;
	address.sin_port = htons(8090);
	address.sin_addr.s_addr = INADDR_ANY;


	int	bind_status = bind( socket_fd, (struct sockaddr*)&address, sizeof(address) );
	if ( bind_status == -1 ) {

		std::cerr << "bind failed" << std::endl;
		safe_close(&socket_fd);
		return 1;
	}

	int flags = fcntl(socket_fd, F_GETFL);
	if (flags == -1)
	{
		std::cerr << "fcntl F_GETFL failed" << std::endl;
		safe_close(&socket_fd);
		return (1);
	}
	if (fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "fcntl F_SETFL failed" << std::endl;
		safe_close(&socket_fd);
		return (1);
	}

	int	listen_status = listen( socket_fd, SOMAXCONN);
	if ( listen_status == -1 )
	{
		safe_close(&socket_fd);
		return 1;
	}

	// int	accepted_fd = accept( socket_fd, NULL, NULL );
	// if ( accepted_fd == -1 )
	// {
	// 	safe_close(&socket_fd);
	// 	return 1;
	// }

	// char buffer[1024];
	// ssize_t received_bytes = 0;

	// Integrate epoll()

	// create epoll_fd
	int epoll_fd = epoll_create(1);

	struct epoll_event epoll_event;
	epoll_event.events = EPOLLIN; // Data available to read (readable)
	epoll_event.data.fd = socket_fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &epoll_event) == -1)
	{
		std::cerr << "epoll_ctl: add failed" << std::endl;
		safe_close(&socket_fd);
		// safe_close(&accepted_fd);
		safe_close(&epoll_fd);
		return (1);
	}

	int kMaxReadyEventsBatchSize = 10;
	struct epoll_event ready_events_list[kMaxReadyEventsBatchSize];

	// === Per-client state (single client for simplicity) ===
	int accepted_fd = -1;
	char request_buffer[8192];
	int request_size = 0;
	const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Length: 24\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello pink t-shirts team";
	int response_length = strlen(response);
	int bytes_sent = 0;
	while (true)
	{
		int num_fds_ready = epoll_wait(epoll_fd, ready_events_list, kMaxReadyEventsBatchSize, -1);
		if (num_fds_ready == -1)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "epoll wait failed" << std::endl;
			safe_close(&socket_fd);
			safe_close(&accepted_fd);
			safe_close(&epoll_fd);
			return (1);
		}

		for (int i = 0; i < num_fds_ready; ++i)
		{
			// retrieve client_fd that is ready
			int ready_client_fd = ready_events_list[i].data.fd;
			uint32_t ready_events = ready_events_list[i].events;

			if (ready_events_list[i].events & (EPOLLHUP | EPOLLERR))
			{
				if (ready_client_fd == accepted_fd)
				{
					safe_close(&accepted_fd);
					request_size = bytes_sent = 0;
				}
				continue;
			}

			if (ready_client_fd == socket_fd)
			{
				while (true)
				{
					accepted_fd = accept(socket_fd, NULL, NULL);
					if (accepted_fd == -1)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						else
						{
							std::cerr << "client accept failed";
							break;
						}
					}
					int flags = fcntl(accepted_fd, F_GETFL);
					if (flags == -1)
					{
						std::cerr << "fcntl F_GETFL failed" << std::endl;
					}
					if (fcntl(accepted_fd, F_SETFL, flags | O_NONBLOCK) == -1)
					{
						std::cerr << "fcntl F_SETFL failed" << std::endl;
					}
					struct epoll_event client_event;
					client_event.data.fd = accepted_fd;
					client_event.events = EPOLLIN;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, accepted_fd, &client_event) == -1)
					{
						std::cerr << "epoll_ctl: add failed" << std::endl;
						safe_close(&accepted_fd);
					}
					request_size = bytes_sent = 0;
					std::cout << "New client connected: " << accepted_fd << std::endl;
				}
			}
			else
			{
				if (ready_events & EPOLLIN)
				{
					char buffer[4096];
					while (true)
					{
						ssize_t bytes = recv(ready_client_fd, buffer, sizeof(buffer), 0);
						if (bytes == 0)
						{
							safe_close(&ready_client_fd);
							break;
						}
						else if (bytes < 0)
						{
							if (errno == EAGAIN || errno == EWOULDBLOCK)
								break;
							safe_close(&ready_client_fd);
							break;
						}
						if (request_size + bytes >= (int)sizeof(request_buffer))
						{
							safe_close(&ready_client_fd);
							request_size = 0;
							break;
						}

						memcpy(request_buffer + request_size, buffer, bytes);
						request_size += bytes;
						request_buffer[request_size] = '\0';

						// Simple HTTP end detection
						if (request_size >= 4 &&
							strstr(request_buffer, "\r\n\r\n"))
						{
							// Switch to write mode
							struct epoll_event ev;
							ev.events = EPOLLOUT;
							ev.data.fd = ready_client_fd;

							epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ready_client_fd, &ev);
							break;
						}
					}
				}

				if (ready_events & EPOLLOUT)
				{
					while (bytes_sent < response_length)
					{
						ssize_t bytes = send(
							ready_client_fd,
							response + bytes_sent,
							response_length - bytes_sent,
							MSG_NOSIGNAL);

						if (bytes < 0)
						{
							if (errno == EAGAIN || errno == EWOULDBLOCK)
								break;

							safe_close(&ready_client_fd);
							// client_fd = -1;
							request_size = bytes_sent = 0;
							break;
						}

						bytes_sent += bytes;
					}

					// Done sending → close connection
					if (bytes_sent >= response_length)
					{
						safe_close(&ready_client_fd);
						// client_fd = -1;
						request_size = bytes_sent = 0;
					}
				}
			}
		}
	}

	close(socket_fd);
	std::cout << "banana3" << std::endl;
	return 0;
}
