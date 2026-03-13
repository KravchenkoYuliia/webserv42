/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 17:42:38 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 12:06:16 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

#include <map>
#include <vector>
#include <inttypes.h> // uint16_t
#include "core/Socket.hpp"
#include "parser/ServerConfig.hpp"
#include "reactor/Reactor.hpp"

class ServerManager {
public:
    // ---------- Constructors / Destructor ----------

    ServerManager(void);
    ~ServerManager(void);

    // ---------- Overloading Operators Methods -------

    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------

    void init( const std::map<uint16_t , std::vector<ServerConfig> >& servers_by_port );
    void run();

protected:

private:
    Reactor                 reactor_;
    std::vector<Socket*>    listening_sockets_;

    ServerManager(const ServerManager& copy);
    ServerManager& operator=(const ServerManager& copy);
};

#endif // SERVER_MANAGER_HPP
