#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
			: path_(kDefaultLoctaionPath) {}
LocationConfig::LocationConfig( const std::string& path )
			: path_( path ) {}


LocationConfig::LocationConfig(const LocationConfig& other) { *this = other; }

LocationConfig&	LocationConfig::operator=(const LocationConfig& other) {

	if (this != &other) {
		this->path_ = other.path_;
		this->root_ = other.root_;
	}
	return *this;
}

void	LocationConfig::setRoot( std::string root ) {
	root_ = root;
}



const std::string&	LocationConfig::getPath() const {
	return path_;
}

const std::string&	LocationConfig::getRoot() const {
	return root_;
}

LocationConfig::~LocationConfig() {}
