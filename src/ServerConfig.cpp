#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& other) { *this = other; }

ServerConfig&	ServerConfig::operator=(const ServerConfig& other) {

	if (this != &other) {
		this->interface_ = other.interface_;
		this->port_= other.port_;
		this->default_server_ = other.default_server_;
		this->location_list_ = other.location_list_;
	}
	return *this;
}


//
//setters
void	ServerConfig::setPort( uint16_t port ) {
	port_ = port;
}

void	ServerConfig::setInterface( std::string interface ) {
	interface_ = interface;
}

void	ServerConfig::setDefaultServer( bool default_server ) {
	default_server_ = default_server;
}

void	ServerConfig::setLocationList( LocationConfig location_config ) {
	location_list_.push_back( location_config );
	//std::cout << "adress of location list is " << &location_list_ << std::endl;
}

//
//getters
const uint16_t&	ServerConfig::getPort() const {
	return port_;
}
const std::string&	ServerConfig::getInterface() const {
	return interface_;
}

const bool&	ServerConfig::getDefaultServer() const {
	return default_server_;
}

const std::vector<LocationConfig>&	ServerConfig::getLocationList() const {
	return location_list_;
}

ServerConfig::~ServerConfig() {}
