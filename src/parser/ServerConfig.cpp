#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
	: port_( kDefaultServerPort ),
	interface_( kDefaultServerInterface ),
	default_server_( false ) {}

ServerConfig::ServerConfig(const ServerConfig& other) { *this = other; }

ServerConfig&	ServerConfig::operator=(const ServerConfig& other) {

	if (this != &other) {
		this->interface_ = other.interface_;
		this->port_= other.port_;
		this->default_server_ = other.default_server_;
		this->location_list_ = other.location_list_;
		this->server_name_ = other.server_name_;
		this->root_ = other.root_;
	}

	return *this;
}


//
//setters
void	ServerConfig::setPort( uint16_t port ) {
	port_ = port;
}

void	ServerConfig::setInterface( const std::string& interface ) {
	interface_ = interface;
}

void	ServerConfig::setDefaultServer( bool default_server ) {
	default_server_ = default_server;
}

void	ServerConfig::setLocationList( const LocationConfig& location_config ) {
	location_list_.push_back( location_config );
	//std::cout << "adress of location list is " << &location_list_ << std::endl;
}

void	ServerConfig::setServerName( std::string server_name ) {
	server_name_.push_back( server_name );
}

void	ServerConfig::setRoot( std::string root ) {
	root_ = root;
}

//
//getters
uint16_t	ServerConfig::getPort() const {
	return port_;
}
std::string	ServerConfig::getInterface() const {
	return interface_;
}

bool	ServerConfig::getDefaultServer() const {
	return default_server_;
}

std::vector<LocationConfig>&	ServerConfig::getLocationList() {
	return location_list_;
}

std::vector<std::string>	ServerConfig::getServerName() const {
	return server_name_;
}

std::string	ServerConfig::getRoot() const {
	return root_;
}

ServerConfig::~ServerConfig() {}
