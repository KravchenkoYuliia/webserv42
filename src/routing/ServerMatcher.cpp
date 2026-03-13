#include "ServerMatcher.hpp"

const ServerConfig&	ServerMatcher::matchServer( const std::vector<ServerConfig>& servers, const std::string& host, uint16_t port ) {

	if ( servers.size() == 1 ) {
		return servers[0];
	}
// check server name 
	if ( host != "" ) {
		for ( std::vector<ServerConfig>::size_type i = 0; i < servers.size(); i++ ) {
			for ( std::vector<std::string>::size_type j = 0; j < servers[i].getServerName().size(); j++ ) {
				if ( servers[i].getServerName()[j] == host )
					return servers[i];
			}
		}
	}
	
//check default_server
	for ( std::vector<ServerConfig>::size_type i = 0; i < servers.size(); i++ ) {
		if ( servers[i].getDefaultServer() == true )
			return servers[i];
	}
	
	(void)port;
	return servers[0];
}
