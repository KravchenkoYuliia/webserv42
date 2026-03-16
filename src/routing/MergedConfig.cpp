#include "MergedConfig.hpp"

MergedConfig::MergedConfig() {}
MergedConfig::MergedConfig( const ServerConfig& server, const LocationConfig& location) :
	server_( server ),
	location_( location )
{}

MergedConfig::MergedConfig( const MergedConfig& other ) { *this = other; }
MergedConfig&	MergedConfig::operator=( const MergedConfig& other ) { 
	if (this != &other) {
		this->server_ = other.server_;
		this->location_ = other.location_;
	}
	return *this;
}


const std::map<int, std::string>&	MergedConfig::getReturn() const {

	if ( location_.getHasReturn() == true )
		return location_.getReturn();
	return server_.getReturn();
}

std::string	MergedConfig::getRoot() const {

	if ( location_.getRoot() != "" )
		return location_.getRoot();
	return server_.getRoot();
}


MergedConfig::~MergedConfig() {}
