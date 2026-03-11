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
		this->return_code_ = other.return_code_;
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

void	ServerConfig::setClientMaxBodySize( long long client_max_body_size ) {
	client_max_body_size_ = client_max_body_size;
}


void	ServerConfig::setReturn( int return_code, const std::string& return_page ) {
	return_code_[return_code] = return_page;
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

long long	ServerConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}

const std::map<int, std::string>&	ServerConfig::getReturn() const {
	return return_code_;
}

bool	ServerConfig::getHasListen() const {
	return has_listen_;
}

bool	ServerConfig::getHasReturn() const {
	return has_return_;
}

std::ostream&	operator<<( std::ostream& out, ServerConfig& s ) {

    out << "\tPort: " << s.getPort() << std::endl
			<< "\tInterface: " << s.getInterface();
			if ( s.getDefaultServer() == true )
				out << " default_server";
		    out << std::endl << "\tServer name: ";
			for ( std::vector<std::string>::size_type i = 0; i < s.getServerName().size(); i++ ) {
				out << s.getServerName()[i] << " ";
			}
			out << std::endl << "\tRoot: " << s.getRoot() << std::endl
				<< "\tIndex: ";
			for ( std::vector<std::string>::size_type in = 0; in < s.getIndex().size(); in++ ) {
				out << s.getIndex()[in] << " ";
			}
			out << std::endl << "\tError page: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = s.getErrorPage().begin(); it != s.getErrorPage().end(); it++ ) {
				out << "\t\t" << it->first << " ---> " << it->second << std::endl;
			}
			out << "\tAutoindex: ";
			if ( s.getAutoindex() == AUTOINDEX_ON )
				out << "on" << std::endl;
			else
				out << "off" << std::endl;
			out << "\tClient_max_body_size: " << s.getClientMaxBodySize() << std::endl;
			if ( !s.getReturn().empty() ) {
				out << "\tReturn: " << std::endl;
				for ( std::map<int, std::string>::const_iterator it = s.getReturn().begin(); it != s.getReturn().end(); it++ ) {
					out << "\t\t" << it->first << " ---> " << it->second << std::endl;
				}
			}
            out << "\tLocation list: " << std::endl;
		    for ( std::vector<LocationConfig>::size_type j = 0; j < s.getLocationList().size(); j++ ) {

		    	out << "\t\tLocation[" << j << "] has:" << std::endl;
		    	out << s.getLocationList()[j];

		    }

    return out;
}


ServerConfig::~ServerConfig() {}
