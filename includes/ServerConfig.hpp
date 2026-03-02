#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <inttypes.h> // uint16_t

class ServerConfig {

public:
	ServerConfig();
	ServerConfig( const ServerConfig& other );
	ServerConfig&	operator = ( const ServerConfig& other );

	~ServerConfig();

	void	setPort( uint16_t port );
	void	setInterface( std::string interface );

	uint16_t	getPort() const;
	std::string	getInterface() const;

private:
	uint16_t	port_;
	std::string		interface_;
};

#endif
