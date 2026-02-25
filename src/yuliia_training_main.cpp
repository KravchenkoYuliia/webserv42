/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   yuliia_training_main.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 15:12:11 by yukravch          #+#    #+#             */
/*   Updated: 2026/02/25 11:57:45 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include_library.hpp"

using namespace std;
int	main() {

	int	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( socket_fd == -1 )
		return 1;

	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return 1;

	struct sockaddr_in	address;
	memset( &address, 0, sizeof(address) );
	address.sin_family = AF_INET;
	address.sin_port = htons(8080);
	address.sin_addr.s_addr = INADDR_ANY;


	int	bind_status = bind( socket_fd, (struct sockaddr*)&address, sizeof(address) );
	if ( bind_status == -1 ) {

		cerr << "bind failed" << endl;
		return 1;
	}

	int	listen_status = listen( socket_fd, SOMAXCONN);
	if ( listen_status == -1 )
		return 1;

	//make non blocking;
	int	flag = fcntl( socket_fd, F_GETFL );
	if ( flag == -1 ) {
		cerr << " socket fcntl SET failed" << endl;
		return 1;
	}
	if ( fcntl( socket_fd, F_SETFL, flag | O_NONBLOCK ) == -1 ) {
		cerr << " socket fcntl GET failed" << endl;
		return 1;
	}

	//epoll 1 STEP
	//
	int	epoll_fd = epoll_create(1); //create an instance of epoll
	if ( epoll_fd == -1 ) {

		cerr << "epoll_create failed" << endl;
		return 1;
	}

	//epoll 2 STEP
	//
	struct	epoll_event	event;
	event.events = EPOLLIN; // tells when data is available to read (readable) instead of checking everything constantly
	event.data.fd = socket_fd;
	if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, socket_fd, &event ) == -1 ) { //register event

		cerr << "epoll_ctl for socket_fd failed" << endl;
		return 1;
	}

	//epoll 3 STEP
	//
	struct	epoll_event	event_for_wait[1024]; //used by kernel

	while ( 1 ) {
		int	fd_ready_for_IO = epoll_wait( epoll_fd, event_for_wait, 1024, -1 );
		//a timeout of -1 causes epoll_wait() to block indefinitely till an event occurs
		//server is "sleeping" till smth happens( CPU is not running )
		if ( fd_ready_for_IO == -1 ){

			cerr << "epoll_wait failed" << endl;
			return 1;
		}

		for ( int i = 0; i < fd_ready_for_IO; i++ ){ //how many events happens while server was waiting

			int	current_fd = event_for_wait[i].data.fd;
			if ( current_fd == socket_fd ) { //there is a new client

				while ( 1 ) {

					int	accepted_client_fd = accept( socket_fd, NULL, NULL );
					if ( accepted_client_fd == -1 ) {

						if ( errno == EAGAIN || errno == EWOULDBLOCK )
							break;
						else{

							cerr << "accept failed" << endl;
							break ;
						}
					}
					cout << "New client connected" << endl;
					//make non blocking;
					int	flag = fcntl( accepted_client_fd, F_GETFL );
					if ( flag == -1 ) {
						cerr << " fcntl SET failed" << endl;
						return 1;
					}
					if ( fcntl( accepted_client_fd, F_SETFL, flag | O_NONBLOCK ) == -1 ) {
						cerr << " fcntl GET failed" << endl;
						return 1;
					}


					struct	epoll_event	event;
					event.events = EPOLLIN;
					event.data.fd = accepted_client_fd;

					if ( epoll_ctl( epoll_fd, EPOLL_CTL_ADD, accepted_client_fd, &event) == -1 ) {
						cerr << " epoll_ctl for accepted_client_fd failed" << endl;
						return 1;
					}


				}

			}
			else { //it's a client that exists already and he send smth

					if ( event_for_wait[i].events & EPOLLIN ) {

						cout << "EPOLLIN condition: client is readable" << endl;
						char buffer[8192];
						ssize_t	received_bytes = recv( current_fd, buffer, sizeof(buffer), 0 );


						if ( received_bytes <= 0 ) {
							cout << ": Client disconnected: " << current_fd << endl;
							close( current_fd );
						}
						else{

							buffer[ received_bytes ] = '\0';

							event.data.fd = current_fd;
							event.events = EPOLLOUT;
							if (epoll_ctl( epoll_fd, EPOLL_CTL_MOD, current_fd, &event) == -1 ) {
								cerr << "epoll_ctl mod failed";
								return 1;
							}


							cout << "Received bytes is " << received_bytes << endl;
							cout << "Message is: " << buffer << endl;
						}

					}
					else if ( event_for_wait[i].events & EPOLLOUT ) {
						cout << "EPOLLOUT condition: client is ready to receive the answer" << endl;
							string	response = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nContent-Length:108\r\n\r\n<!DOCTYPE html>\n<html>\n<body>\n<h1>Welcome to Webserv</h1>\n<p>Team T-shirt rose.</p>\n</body>\n</html>";
							cout << "Sending response html" << endl;


							send( current_fd, response.c_str(), response.length(), 0);
							close( current_fd );
					}
			}
		}
	}

	close(socket_fd);
	return 0;
}
