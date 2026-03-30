#include "routing/ServerMatcher.hpp"

const ServerConfig&	ServerMatcher::matchServer( const std::vector<ServerConfig>& servers, const std::string& host ) {

    if ( servers.size() == 1 ) {
        return servers[0];
    }
// check server name
    if ( host != "" ) {
        for ( std::vector<ServerConfig>::size_type i = 0; i < servers.size(); i++ ) {
            const std::vector<std::string>& names = servers[i].getServerName();
            for ( std::vector<std::string>::size_type j = 0; j < names.size(); j++ ) {
                if ( names[j] == host )
                    return servers[i];
            }
        }
    }

//check default_server
    for ( std::vector<ServerConfig>::size_type i = 0; i < servers.size(); i++ ) {
        if ( servers[i].getDefaultServer() == true )
            return servers[i];
    }
    return servers[0];
}
