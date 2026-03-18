#include "LocationConfig.hpp"

LocationConfig::LocationConfig()
			: autoindex_( AUTOINDEX_NOT_SPECIFIED ),
			path_(kDefaultLocationPath),
			root_( "" ),
			client_max_body_size_(kNotSpecified),
			has_return_( false ),
			upload_allowed_( false ),
			upload_location_( "" )
{}
LocationConfig::LocationConfig( const std::string& path )
			: autoindex_( AUTOINDEX_NOT_SPECIFIED ),
			path_( path ),
			root_( "" ),
			client_max_body_size_(kNotSpecified),
			has_return_( false ),
			upload_allowed_( false ),
			upload_location_( "" )
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
		this->return_code_ = other.return_code_;
		this->has_return_ = other.has_return_;
		this->upload_allowed_ = other.upload_allowed_;
		this->upload_location_ = other.upload_location_;
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

void	LocationConfig::setClientMaxBodySize( long long client_max_body_size ) {
	client_max_body_size_ = client_max_body_size;
}

void	LocationConfig::setAllowedMethods( const std::string& allowed_methods ) {
	allowed_methods_.push_back( allowed_methods );
}

void	LocationConfig::setReturn( int return_code, const std::string& return_page ) {
	return_code_[return_code] = return_page;
}

void	LocationConfig::setHasReturn() {
	has_return_ = true;
}

void	LocationConfig::setUploadAllowed() {
	upload_allowed_ = true;
}

void	LocationConfig::setUploadLocation( const std::string& upload_location ) {
	upload_location_ = upload_location;
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

long long	LocationConfig::getClientMaxBodySize() const {
	return client_max_body_size_;
}

const std::vector<std::string>&	LocationConfig::getAllowedMethods() const {
	return allowed_methods_;
}

const std::map<int, std::string>&	LocationConfig::getReturn() const {
	return return_code_;
}

bool	LocationConfig::getHasReturn() const {
	return has_return_;
}

bool	LocationConfig::getUploadAllowed() const {
	return upload_allowed_;
}

const std::string&	LocationConfig::getUploadLocation() const {
	return upload_location_;
}


std::ostream&	operator<<( std::ostream& out, const LocationConfig& l ) {
	    out << "\t\t\t\tpath: "
			<< l.getPath() << std::endl;
		if ( l.getRoot() != "" )
			out << "\t\t\t\troot: " << l.getRoot() << std::endl;
		if ( !l.getIndex().empty() ) {
			out << "\t\t\t\tindex: ";
			for ( std::vector<std::string>::size_type ind = 0; ind < l.getIndex().size(); ind++ ) {
				out << l.getIndex()[ind] << " ";
			}
			out << std::endl;
		}
		if ( !l.getErrorPage().empty() ) {
			out << "\t\t\t\terror page: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = l.getErrorPage().begin(); it != l.getErrorPage().end(); it++ ) {
				out << "\t\t\t\t\t" << it->first << " ---> " << it->second << std::endl;
			}
		}
    if ( l.getAutoindex() != AUTOINDEX_NOT_SPECIFIED ) {
			out << "\t\t\t\tautoindex: ";
		if ( l.getAutoindex() == true )
			out << "on" << std::endl;
		else if ( l.getAutoindex() == false )
			out << "off" << std::endl;
    }
		if ( l.getClientMaxBodySize() != 0)
			out << "\t\t\t\tclient_max_body_size: " << l.getClientMaxBodySize() << std::endl;
		if ( !l.getAllowedMethods().empty() ) {
			out << "\t\t\t\tallowed methods: ";
			for ( std::vector<std::string>::size_type met = 0; met < l.getAllowedMethods().size(); met++ ) {
				out << l.getAllowedMethods()[met] << " ";
			}
			std::cout << std::endl;
		}
		if ( !l.getReturn().empty() ) {
			out << "\t\t\t\treturn: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = l.getReturn().begin(); it != l.getReturn().end(); it++ ) {
				out << "\t\t\t\t\t" << it->first << " ---> " << it->second << std::endl;
			}
		}
		out << "\t\t\t\tupload allowed: ";
		if ( l.getUploadAllowed() == true ) {
			out << "YES" << std::endl << "\t\t\t\tupload_location: " << l.getUploadLocation() << std::endl;
		}
		else
			out << "NO" << std::endl;

	return out;
}

LocationConfig::~LocationConfig() {}
