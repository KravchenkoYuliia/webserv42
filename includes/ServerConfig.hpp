#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "webserv.hpp"

class ServerConfig {

public:
	ServerConfig();
	ServerConfig( const ServerConfig& other ) ; //TODO: keep public or private?
	ServerConfig&	operator = ( const ServerConfig& other ); //TODO: keep public or private?

	~ServerConfig();

	void	setPort( unsigned int port );
	void	setInterface( unsigned int interface );

	unsigned int	getPort() const;
	std::string	getInterface() const;

private:
	unsigned int	port_;
	std::string		interface_;
};

#endif
