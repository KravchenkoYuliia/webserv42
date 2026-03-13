#ifndef SERVERMATCHER_HPP
#define SERVERMATCHER_HPP

#include <iostream>
#include "ServerConfig.hpp" 

class ServerMatcher {

public:
	static const ServerConfig&	matchServer( const std::vector<ServerConfig>& servers, const std::string& host, uint16_t port );

private:

	ServerMatcher();
	ServerMatcher( const ServerMatcher& other );
	ServerMatcher&	operator = ( const ServerMatcher& other );
	~ServerMatcher();

};

#endif
