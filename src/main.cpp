/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 12:08:39 by yukravch          #+#    #+#             */
/*   Updated: 2026/04/07 13:15:41 by yukravch         ###   ########.fr       */
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
#include "core/SignalHandler.hpp"
#include "utils/Utils.hpp"
#include "core/ServerManager.hpp"

char** global_env;
int main(int argc, char **argv, char **env )
{

    if ( argc != 2 )
    {
        std::cerr << "Usage: ./webserv <config_file> " << std::endl;
        return 1;
    }
	global_env = env;
    try {
        std::signal(SIGINT, handle_sigint);
        ServerManager server_manager;
        ConfigParser configParser( argv[1] );
        std::map<uint16_t, std::vector<ServerConfig> >& map = configParser.getMapOfPortWithServers();
        server_manager.init(map);
        server_manager.run();
    } catch (const std::exception& e)
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}
