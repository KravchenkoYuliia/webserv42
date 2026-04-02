/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 12:08:39 by yukravch          #+#    #+#             */
/*   Updated: 2026/04/07 15:27:58 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <exception>
#include "ConfigParser.hpp"
#include "core/Socket.hpp"
#include "core/SignalHandler.hpp"
#include "utils/Utils.hpp"
#include "core/ServerManager.hpp"

int main(int argc, char **argv)
{

    if ( argc != 2 )
    {
        std::cerr << "Usage: ./webserv <config_file> " << std::endl;
        return 1;
    }
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
