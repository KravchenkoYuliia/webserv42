#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
			: path_(kDefaultLoctaionPath),
			client_max_body_size_(0)
{}
LocationConfig::LocationConfig( const std::string& path )
			: path_( path ),
			client_max_body_size_(0)
{}


LocationConfig::LocationConfig(const LocationConfig& other) { *this = other; }

LocationConfig&	LocationConfig::operator=(const LocationConfig& other) {

	if (this != &other) {
		this->path_ = other.path_;
		this->root_ = other.root_;
		this->index_ = other.index_;
		this->error_page_ = other.error_page_;
		this->autoindex_ = other.autoindex_;
		this->client_max_body_size_ = other.client_max_body_size_;
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

void	LocationConfig::setErrorPage( int error_nb, const std::string& error_page ) {
	error_page_[error_nb] = error_page;
}

void	LocationConfig::setAutoindex( bool autoindex ) {
	autoindex_ = autoindex;
}

void	LocationConfig::setClientMaxBodySize( unsigned long client_max_body_size ) {
	client_max_body_size_ = client_max_body_size;
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

const std::map<int, std::string>&	LocationConfig::getErrorPage() const {
	return error_page_;
}

bool	LocationConfig::getAutoindex() const {
	return autoindex_;
}

unsigned long	LocationConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}


LocationConfig::~LocationConfig() {}
