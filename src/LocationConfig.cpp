#include "LocationConfig.hpp"

LocationConfig::LocationConfig() {}
LocationConfig::LocationConfig( std::string path )
			: path_( path ) {}


LocationConfig::LocationConfig(const LocationConfig& other) { *this = other; }

LocationConfig&	LocationConfig::operator=(const LocationConfig& other) {

	if (this != &other) {
		this->path_ = other.path_;
	}
	return *this;
}

const std::string&	LocationConfig::getPath() const {
	return path_;
}
LocationConfig::~LocationConfig() {}
