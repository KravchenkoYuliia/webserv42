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

	if ( location_.getHasReturn() == true ) {
		return location_.getReturn();
	}
	return server_.getReturn();
}

std::string	MergedConfig::getRoot() const {

	if ( location_.getRoot() != "" )
		return location_.getRoot();
	return server_.getRoot();
}

const std::vector<std::string>&		MergedConfig::getMethods() const {
	return location_.getAllowedMethods();
}

const std::map<int, std::string>&	MergedConfig::getErrorPage() const {

	if ( !location_.getErrorPage().empty() )
		return location_.getErrorPage();
	return server_.getErrorPage();
}

size_t	MergedConfig::getMaxBodySize() const {

	if ( location_.getClientMaxBodySize() != -1 )
		return location_.getClientMaxBodySize();
	return server_.getClientMaxBodySize();
}

const std::string&	MergedConfig::getPath() const {
	return location_.getPath();
}

const std::vector<std::string>&	MergedConfig::getIndex() const {
	if ( !location_.getIndex().empty() )
		return location_.getIndex();
	return server_.getIndex();
}

AutoindexType	MergedConfig::getAutoindex() const {
	if ( location_.getAutoindex() != AUTOINDEX_NOT_SPECIFIED )
		return location_.getAutoindex();
	return server_.getAutoindex();
}

bool	MergedConfig::getUploadAllowed() const {
	return location_.getUploadAllowed();
}

const std::string&	MergedConfig::getUploadLocation() const {
	return location_.getUploadLocation();
}

MergedConfig::~MergedConfig() {}
