#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
			: autoindex_( AUTOINDEX_NOT_SPECIFIED ),
			path_(kDefaultLocationPath),
			client_max_body_size_(0),
            has_return_( false )
{}
LocationConfig::LocationConfig( const std::string& path )
			: autoindex_( AUTOINDEX_NOT_SPECIFIED ),
			path_( path ),
			client_max_body_size_(0),
            has_return_( false )
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
		this->allowed_methods_ = other.allowed_methods_;
		this->return_page_ = other.return_page_;
        this->has_return_ = other.has_return_;
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

void	LocationConfig::setAutoindex( AutoindexType autoindex ) {
	autoindex_ = autoindex;
}

void	LocationConfig::setClientMaxBodySize( unsigned long client_max_body_size ) {
	client_max_body_size_ = client_max_body_size;
}

void	LocationConfig::setAllowedMethods( const std::string& allowed_methods ) {
	allowed_methods_.push_back( allowed_methods );
}

void	LocationConfig::setReturnPage( int return_code, const std::string& return_page ) {
	return_page_[return_code] = return_page;
}

void	LocationConfig::setHasReturn() {
	has_return_ = true;
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

AutoindexType	LocationConfig::getAutoindex() const {
	return autoindex_;
}

unsigned long	LocationConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}

std::vector<std::string>	LocationConfig::getAllowedMethods() const {
	return allowed_methods_;
}

const std::map<int, std::string>&	LocationConfig::getReturnPage() const {
	return return_page_;
}

bool	LocationConfig::getHasReturn() const {
	return has_return_;
}

LocationConfig::~LocationConfig() {}
