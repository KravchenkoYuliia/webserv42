#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include "webserv.hpp"
#include "LocationConfig.hpp" 

class LocationConfig;
class ServerConfig {

public:
	ServerConfig();
	ServerConfig( const ServerConfig& other );
	ServerConfig&	operator = ( const ServerConfig& other );

	~ServerConfig();

	void	setPort( uint16_t port );
	void	setInterface( std::string interface );
	void	setDefaultServer( bool default_server );
	void	setLocationList( LocationConfig location_config );

	const uint16_t&			getPort() const;
	const std::string&			getInterface() const;
	const bool&		 		getDefaultServer() const;
	const std::vector<LocationConfig>&	getLocationList() const;

	
private:
	uint16_t					port_;
	std::string					interface_;
	bool						default_server_;
	std::vector<LocationConfig>			location_list_;
};

#endif
