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


void	ServerConfig::setPort( unsigned int port ) {
	port_ = port;
}
void	ServerConfig::setInterface( unsigned int interface ) {
	interface_ = interface;
}

unsigned int	ServerConfig::getPort() const {
	return port_;
}
std::string	ServerConfig::getInterface() const {
	return interface_;
}


ServerConfig::~ServerConfig() {}
