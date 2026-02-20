/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 15:12:11 by yukravch          #+#    #+#             */
/*   Updated: 2026/02/20 16:39:30 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream> 
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <cstring> //for memset 
#include <unistd.h>

int	main(){

	int	socket_fd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( socket_fd == -1 )
		return 1;

	int opt = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		return 1;

	struct sockaddr_in	address;
	std::memset( &address, 0, sizeof(address) );
	address.sin_family = AF_INET;
	address.sin_port = htons(8090);
	address.sin_addr.s_addr = INADDR_ANY;


	int	bind_status = bind( socket_fd, (struct sockaddr*)&address, sizeof(address) );
	if ( bind_status == -1 ) {

		std::cerr << "bind failed" << std::endl;
		return 1;
	}

	int	listen_status = listen( socket_fd, SOMAXCONN);
	if ( listen_status == -1 )
		return 1;

	int	accepted_fd = accept( socket_fd, NULL, NULL );
	if ( accepted_fd == -1 )
		return 1;


	char	buffer[1024];
	ssize_t		received_bytes = 0;

	while ( 1 ) {

		received_bytes = recv( accepted_fd, &buffer, 1024, 0 );
		std::cout << "Received bytes is " << received_bytes << std::endl;
		if ( received_bytes == -1 )
			return 1;
		std::cout << "Successfully received " << received_bytes << " bytes from client" << std::endl << "Message is:  " << buffer << std::endl;

	}

	close(socket_fd);	
	std::cout << "banana3" << std::endl;
	return 0;
}
