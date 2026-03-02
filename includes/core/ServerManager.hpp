/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 17:42:38 by jgossard          #+#    #+#             */
/*   Updated: 2026/02/27 19:01:38 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

// #include <vector>
#include <inttypes.h> // uint16_t
#include "reactor/Reactor.hpp"
#include "core/Socket.hpp"

// class ServerConfig; // TODO: to keep it as forward declaration or as an include?

class ServerManager {
public:
    // ---------- Constructors / Destructor ----------

    ServerManager(void);
    ServerManager(const ServerManager& copy);
    ~ServerManager(void);

    // ---------- Overloading Operators Methods -------

    ServerManager& operator=(const ServerManager& copy);

    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------
    // void init( std::vector<ServerConfig> );
    void init( uint16_t port );
    void run();

protected:

private:
    Reactor reactor_;
    Socket  server_socket_;
};

#endif // SERVER_MANAGER_HPP
