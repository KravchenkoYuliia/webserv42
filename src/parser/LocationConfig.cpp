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
		this->index_ = other.index_;
	}
	return *this;
}

//setters
void	LocationConfig::setRoot( const std::string& root ) {
	root_ = root;
}

void	LocationConfig::setIndex( const std::string& index ) {
	index_.push_back( index );
}


//getters	
const std::string&	LocationConfig::getPath() const {
	return path_;
}

const std::string&	LocationConfig::getRoot() const {
	return root_;
}
const std::vector<std::string>&		LocationConfig::getIndex() const {
	return index_;
}

LocationConfig::~LocationConfig() {}
