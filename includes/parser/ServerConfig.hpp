#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <inttypes.h> // uint16_t

#include "LocationConfig.hpp"

class LocationConfig;
class ServerConfig {

#define kDefaultServerPort 8080
#define kDefaultServerInterface "0.0.0.0"
#define kDefaultRoot "html"
#define kDefaultIndex "index.html"


public:
	ServerConfig();
	ServerConfig( const ServerConfig& other );
	ServerConfig&	operator = ( const ServerConfig& other );

	~ServerConfig();

	void	setPort( uint16_t port );
	void	setInterface( const std::string& interface );
	void	setDefaultServer( bool default_server );
	void	setLocationList( const LocationConfig& location_config );
	void	setServerName( const std::string& server_name );
	void	setRoot( const std::string& root );
	void	setIndex( const std::string& index );

	uint16_t			            getPort() const;
	const std::string&			            getInterface() const;
	bool		 		            getDefaultServer() const;
	std::vector<LocationConfig>&	getLocationList();
	const std::vector<std::string>&	    getServerName() const;
	const std::string&	                    getRoot() const;
	const std::vector<std::string>&	    getIndex() const;


private:
	uint16_t			        port_;
	std::string			        interface_;
	bool				        default_server_;
	std::vector<LocationConfig> location_list_;
	std::vector<std::string>	server_name_;
	std::string				    root_;
	std::vector<std::string>	index_;
};

#endif
