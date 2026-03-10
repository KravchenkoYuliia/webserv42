/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 16:21:31 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Utils.hpp"
#include <fcntl.h>          // fcntl
#include <stdexcept>
#include <iostream>

void Utils::setNonBlocking(int fd)
{
    int	flag = fcntl( fd, F_GETFL );
    if ( flag == -1 )
        throw std::runtime_error("fcntl GET failed");
    if ( fcntl( fd, F_SETFL, flag | O_NONBLOCK ) == -1 )
        throw std::runtime_error("fcntl SET failed");
}

void Utils::printServersContent( std::vector<ServerConfig>& servers ) {

	for ( std::vector<ServerConfig>::size_type i = 0; i < servers.size(); i++ ) {
		std::cout << "Server[" << i << "] has:" << std::endl
			<< servers[i];
	}
}
