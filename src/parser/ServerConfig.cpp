#include "ServerConfig.hpp"

ServerConfig::ServerConfig()
	: port_( kDefaultServerPort ),
	interface_( kDefaultServerInterface ),
	default_server_( false ),
	has_listen_( false ),
    has_return_( false ),
	root_( kDefaultRoot ),
	autoindex_( AUTOINDEX_OFF ),
	client_max_body_size_( kDefaultClientMaxBodySize )
{}

ServerConfig::ServerConfig(const ServerConfig& other) { *this = other; }

ServerConfig&	ServerConfig::operator=(const ServerConfig& other) {

	if (this != &other) {
		this->interface_ = other.interface_;
		this->port_= other.port_;
		this->default_server_ = other.default_server_;
		this->has_listen_ = other.has_listen_;
        this->has_return_ = other.has_return_;
		this->location_list_ = other.location_list_;
		this->server_name_ = other.server_name_;
		this->root_ = other.root_;
		this->index_ = other.index_;
		this->error_page_ = other.error_page_;
		this->autoindex_ = other.autoindex_;
		this->client_max_body_size_ = other.client_max_body_size_;
		this->return_page_ = other.return_page_;
	}

	return *this;
}


//
//setters
void	ServerConfig::setPort( uint16_t port ) {
	port_ = port;
}

void	ServerConfig::setInterface( const std::string& interface ) {
	interface_ = interface;
}

void	ServerConfig::setDefaultServer( bool default_server ) {
	default_server_ = default_server;
}

void	ServerConfig::setLocationList( const LocationConfig& location_config ) {
	location_list_.push_back( location_config );
}

void	ServerConfig::setServerName( const std::string& server_name ) {
	server_name_.push_back( server_name );
}

void	ServerConfig::setRoot( const std::string& root ) {
	root_ = root;
}

void	ServerConfig::setIndex( const std::string& index ) {
	index_.push_back( index );
}

void	ServerConfig::setErrorPage( int error_nb, const std::string& error_page ) {
	error_page_[error_nb] = error_page;
}

void	ServerConfig::setAutoindex( AutoindexType autoindex ) {
	autoindex_ = autoindex;
}

void	ServerConfig::setClientMaxBodySize( unsigned long client_max_body_size ) {
	client_max_body_size_ = client_max_body_size;
}


void	ServerConfig::setReturnPage( int return_code, const std::string& return_page ) {
	return_page_[return_code] = return_page;
}

void	ServerConfig::setHasListen() {
	has_listen_ = true;
}

void	ServerConfig::setHasReturn() {
	has_return_ = true;
}

//
//getters
uint16_t	ServerConfig::getPort() const {
	return port_;
}
const std::string&	ServerConfig::getInterface() const {
	return interface_;
}

bool	ServerConfig::getDefaultServer() const {
	return default_server_;
}


std::vector<LocationConfig>&	ServerConfig::getLocationList() {
	return location_list_;
}

const std::vector<std::string>&	ServerConfig::getServerName() const {
	return server_name_;
}

const std::string&	ServerConfig::getRoot() const {
	return root_;
}

const std::vector<std::string>&	    ServerConfig::getIndex() const {
	return index_;
}

const std::map<int, std::string>&	ServerConfig::getErrorPage() const {
	return error_page_;
}

AutoindexType	ServerConfig::getAutoindex() const {
	return autoindex_;
}

unsigned long	ServerConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}

const std::map<int, std::string>&	ServerConfig::getReturnPage() const {
	return return_page_;
}

bool	ServerConfig::getHasListen() const {
	return has_listen_;
}

bool	ServerConfig::getHasReturn() const {
	return has_return_;
}

ServerConfig::~ServerConfig() {}
