#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& other) { *this = other; }

ServerConfig&	ServerConfig::operator=(const ServerConfig& other) {

	if (this != &other) {
		this->interface_ = other.interface_;
		this->port_= other.port_;
	}
	return *this;
}


void	ServerConfig::setPort( uint16_t port ) {
	port_ = port;
}
void	ServerConfig::setInterface( std::string interface ) {
	interface_ = interface;
}

uint16_t	ServerConfig::getPort() const {
	return port_;
}
std::string	ServerConfig::getInterface() const {
	return interface_;
}


ServerConfig::~ServerConfig() {}
