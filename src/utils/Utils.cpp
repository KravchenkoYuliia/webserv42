/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 15:35:10 by yukravch         ###   ########.fr       */
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
		std::cout << "Server[" << i << "]" << " has: " << std::endl
			<< "\tPort: " << servers[i].getPort() << std::endl
			<< "\tInterface: " << servers[i].getInterface();
			if ( servers[i].getDefaultServer() == true )
				std::cout << " default_server";
			std::cout << std::endl << "\tServer name: ";
			for ( std::vector<std::string>::size_type s = 0; s < servers[i].getServerName().size(); s++ ) {
				std::cout << servers[i].getServerName()[s] << " ";
			}
			std::cout << std::endl << "\tRoot: " << servers[i].getRoot() << std::endl
				<< "\tIndex: ";
			for ( std::vector<std::string>::size_type in = 0; in < servers[i].getIndex().size(); in++ ) {
				std::cout << servers[i].getIndex()[in] << " ";
			}
			std::cout << std::endl << "\tError page: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = servers[i].getErrorPage().begin(); it != servers[i].getErrorPage().end(); it++ ) {
				std::cout << "\t\t" << it->first << " ---> " << it->second << std::endl;
			}
			std::cout << "\tAutoindex: ";
			if ( servers[i].getAutoindex() == AUTOINDEX_ON )
				std::cout << "on" << std::endl;
			else
				std::cout << "off" << std::endl;
			std::cout << "\tClient_max_body_size: " << servers[i].getClientMaxBodySize() << std::endl;
			if ( !servers[i].getReturn().empty() ) {
				std::cout << "\tReturn: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = servers[i].getReturn().begin(); it != servers[i].getReturn().end(); it++ ) {
					std::cout << "\t\t" << it->first << " ---> " << it->second << std::endl;
				}
			}

            Utils::printLocationBlock( servers[i].getLocationList());
	}
}

void    Utils::printLocationBlock( std::vector<LocationConfig>& location_list ) {

 		std::cout << "\tLocation list: " << std::endl;
		for ( std::vector<LocationConfig>::size_type j = 0; j < location_list.size(); j++ ) {

			std::cout << "\t\tLocation[" << j << "] has:" << std::endl << "				path: "
				<< location_list[j].getPath() << std::endl;
			if ( location_list[j].getRoot() != "" )
				std::cout << "\t\t\t\troot: " << location_list[j].getRoot() << std::endl;
			if ( !location_list[j].getIndex().empty() ) {
				std::cout << "\t\t\t\tindex: ";
				for ( std::vector<std::string>::size_type ind = 0; ind < location_list[j].getIndex().size(); ind++ ) {
					std::cout << location_list[j].getIndex()[ind] << " ";
				}
				std::cout << std::endl;
			}
			if ( !location_list[j].getErrorPage().empty() ) {
				std::cout << "\t\t\t\terror page: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = location_list[j].getErrorPage().begin(); it != location_list[j].getErrorPage().end(); it++ ) {
					std::cout << "\t\t\t\t\t" << it->first << " ---> " << it->second << std::endl;
				}
			}
            if ( location_list[j].getAutoindex() != AUTOINDEX_NOT_SPECIFIED ) {
				std::cout << "\t\t\t\tautoindex: ";
			if ( location_list[j].getAutoindex() == true )
				std::cout << "on" << std::endl;
			else if ( location_list[j].getAutoindex() == false )
				std::cout << "off" << std::endl;
            }
			if ( location_list[j].getClientMaxBodySize() != 0)
				std::cout << "\t\t\t\tclient_max_body_size: " << location_list[j].getClientMaxBodySize() << std::endl;
			if ( !location_list[j].getAllowedMethods().empty() ) {
				std::cout << "\t\t\t\tallowed methods: ";
				for ( std::vector<std::string>::size_type met = 0; met < location_list[j].getAllowedMethods().size(); met++ ) {
					std::cout << location_list[j].getAllowedMethods()[met] << " ";
				}
				std::cout << std::endl;
			}
			if ( !location_list[j].getReturn().empty() ) {
				std::cout << "\t\t\t\treturn: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = location_list[j].getReturn().begin(); it != location_list[j].getReturn().end(); it++ ) {
					std::cout << "\t\t\t\t\t" << it->first << " ---> " << it->second << std::endl;
				}
			}
		}
}
