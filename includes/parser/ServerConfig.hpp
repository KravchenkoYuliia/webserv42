#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <inttypes.h> // uint16_t
#include <map>

#include "LocationConfig.hpp"

static const int kDefaultServerPort = 8080;
static const std::string kDefaultServerInterface = "0.0.0.0";
static const std::string kDefaultRoot = "html";
static const std::string kDefaultIndex = "index.html";
static const int kDefaultClientMaxBodySize = 1024 * 1024; //equivalent to 1M

class LocationConfig;
class ServerConfig {

public:
	ServerConfig();
	ServerConfig( const ServerConfig& other );
	ServerConfig&	operator = ( const ServerConfig& other );

	~ServerConfig();

	void	setPort( uint16_t port );
	void	setInterface( const std::string& interface );
	void	setDefaultServer( bool default_server );
	void	setHasListen();
	void	setLocationList( const LocationConfig& location_config );
	void	setServerName( const std::string& server_name );
	void	setRoot( const std::string& root );
	void	setIndex( const std::string& index );
	void	setErrorPage( int error_nb, const std::string& error_page );
	void	setAutoindex( AutoindexType autoindex_ );
	void	setClientMaxBodySize( unsigned long client_max_body_size );
	void	setReturn( int return_code, const std::string& return_page );
    void	setHasReturn();

//getters
	uint16_t			            getPort() const;
	const std::string&			            getInterface() const;
	bool					getDefaultServer() const;
	bool					getHasListen() const;
	std::vector<LocationConfig>&	getLocationList();
	const std::vector<std::string>&	    getServerName() const;
	const std::string&	                    getRoot() const;
	const std::vector<std::string>&	    getIndex() const;
	const std::map<int, std::string>&	getErrorPage() const;
	AutoindexType		 		            getAutoindex() const;
	unsigned long			getClientMaxBodySize() const;
	const std::map<int, std::string>&	getReturn() const;
    bool					getHasReturn() const;

private:
	uint16_t			        port_;
	std::string			        interface_;
	bool				        default_server_;
	bool					has_listen_;
    bool                    has_return_;
	std::vector<LocationConfig> location_list_;
	std::vector<std::string>	server_name_;
	std::string				    root_;
	std::vector<std::string>	index_;
	std::map<int, std::string>	error_page_;
	AutoindexType				autoindex_;
	unsigned long			client_max_body_size_;
	std::map<int, std::string>	return_code_;
};

#endif
