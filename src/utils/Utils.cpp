/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 17:54:23 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/05 15:56:58 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/Utils.hpp"
#include <fcntl.h>          // fcntl
#include <stdexcept>
#include <iostream>

#define ONE_TAB "	"
#define TWO_TABS "		"
#define FOUR_TABS "				"
#define FIVE_TABS "					"

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
			<< ONE_TAB << "Port: " << servers[i].getPort() << std::endl
			<< ONE_TAB << "Interface: " << servers[i].getInterface();
			if ( servers[i].getDefaultServer() == true )
				std::cout << " default_server";
			std::cout << std::endl << ONE_TAB << "Server name: ";
			for ( std::vector<std::string>::size_type s = 0; s < servers[i].getServerName().size(); s++ ) {
				std::cout << servers[i].getServerName()[s] << " ";
			}
			std::cout << std::endl << ONE_TAB << "Root: " << servers[i].getRoot() << std::endl
				<< ONE_TAB << "Index: ";
			for ( std::vector<std::string>::size_type in = 0; in < servers[i].getIndex().size(); in++ ) {
				std::cout << servers[i].getIndex()[in] << " ";
			}
			std::cout << std::endl << ONE_TAB << "Error page: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = servers[i].getErrorPage().begin(); it != servers[i].getErrorPage().end(); it++ ) {
				std::cout << TWO_TABS << it->first << " ---> " << it->second << std::endl;
			}
			std::cout << ONE_TAB << "Autoindex: ";
			if ( servers[i].getAutoindex() == AUTOINDEX_ON )
				std::cout << "on" << std::endl;
			else
				std::cout << "off" << std::endl;
			std::cout << ONE_TAB << "Client_max_body_size: " << servers[i].getClientMaxBodySize() << std::endl;
			if ( !servers[i].getReturnPage().empty() ) {
				std::cout << ONE_TAB << "Return: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = servers[i].getReturnPage().begin(); it != servers[i].getReturnPage().end(); it++ ) {
					std::cout << TWO_TABS << it->first << " ---> " << it->second << std::endl;
				}
			}

            Utils::printLocationBlock( servers[i].getLocationList());
	}
}

void    Utils::printLocationBlock( std::vector<LocationConfig>& location_list ) {

 		std::cout << ONE_TAB << "Location list: " << std::endl;
		for ( std::vector<LocationConfig>::size_type j = 0; j < location_list.size(); j++ ) {

			std::cout << TWO_TABS << "Location[" << j << "] has:" << std::endl << "				path: "
				<< location_list[j].getPath() << std::endl;
			if ( location_list[j].getRoot() != "" )
				std::cout << FOUR_TABS << "root: " << location_list[j].getRoot() << std::endl;
			if ( !location_list[j].getIndex().empty() ) {
				std::cout << FOUR_TABS << "index: ";
				for ( std::vector<std::string>::size_type ind = 0; ind < location_list[j].getIndex().size(); ind++ ) {
					std::cout << location_list[j].getIndex()[ind] << " ";
				}
				std::cout << std::endl;
			}
			if ( !location_list[j].getErrorPage().empty() ) {
				std::cout << FOUR_TABS << "error page: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = location_list[j].getErrorPage().begin(); it != location_list[j].getErrorPage().end(); it++ ) {
					std::cout << FIVE_TABS << it->first << " ---> " << it->second << std::endl;
				}
			}
            if ( location_list[j].getAutoindex() != AUTOINDEX_NOT_SPECIFIED ) {
				std::cout << FOUR_TABS << "autoindex: ";
			if ( location_list[j].getAutoindex() == true )
				std::cout << "on" << std::endl;
			else if ( location_list[j].getAutoindex() == false )
				std::cout << "off" << std::endl;
            }
			if ( location_list[j].getClientMaxBodySize() != 0)
				std::cout << FOUR_TABS << "client_max_body_size: " << location_list[j].getClientMaxBodySize() << std::endl;
			if ( !location_list[j].getAllowedMethods().empty() ) {
				std::cout << FOUR_TABS << "allowed methods: ";
				for ( std::vector<std::string>::size_type met = 0; met < location_list[j].getAllowedMethods().size(); met++ ) {
					std::cout << location_list[j].getAllowedMethods()[met] << " ";
				}
				std::cout << std::endl;
			}
			if ( !location_list[j].getReturnPage().empty() ) {
				std::cout << FOUR_TABS << "return: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = location_list[j].getReturnPage().begin(); it != location_list[j].getReturnPage().end(); it++ ) {
					std::cout << FIVE_TABS << it->first << " ---> " << it->second << std::endl;
				}
			}
		}
}
