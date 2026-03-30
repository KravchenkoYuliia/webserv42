#ifndef SERVERMATCHER_HPP
#define SERVERMATCHER_HPP

#include <iostream>
#include "parser/ServerConfig.hpp"

class ServerMatcher {

public:
	static const ServerConfig&	matchServer( const std::vector<ServerConfig>& servers, const std::string& host );

private:

	ServerMatcher();
	ServerMatcher( const ServerMatcher& other );
	ServerMatcher&	operator = ( const ServerMatcher& other );
	~ServerMatcher();

};

#endif
