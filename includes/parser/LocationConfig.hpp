#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>

static const std::string kDefaultLocationPath = "/";
static const int kNotSpecified = -1;

enum AutoindexType {
	AUTOINDEX_OFF,
	AUTOINDEX_ON,
	AUTOINDEX_NOT_SPECIFIED
};

class LocationConfig {

public:

	LocationConfig();
	LocationConfig( const std::string& path );
	LocationConfig(const LocationConfig& other);
	LocationConfig&	operator = (const LocationConfig& other);
	~LocationConfig();

//setters

	void	setRoot( const std::string& root );
	void	setIndex( const std::string& index );
	void	setErrorPage( int error_nb, const std::string& error_page );
	void	setAutoindex( AutoindexType autoindex_ );
	void	setClientMaxBodySize( long long client_max_body_size );
	void	setAllowedMethods( const std::string& allowed_methods );
	void	setReturn( int return_code, const std::string& return_page );
	void	setHasReturn();
	void	setUploadAllowed();
	void	setUploadLocation( const std::string& upload_location );
	void	setCgi( const std::string& extension, const std::string& path );


//getters

	const std::string&					getPath() const;
	const std::string&					getRoot() const;
	const std::vector<std::string>&		getIndex() const;
	const std::map<int, std::string>&	getErrorPage() const;
	AutoindexType						getAutoindex() const;
	long long							getClientMaxBodySize() const;
	const std::vector<std::string>&		getAllowedMethods() const;
	const std::map<int, std::string>&	getReturn() const;
	bool                                getHasReturn() const;
	bool								getUploadAllowed() const;
	const std::string&					getUploadLocation() const;
	const std::map<std::string, std::string>&	getCgi() const;

private:
	AutoindexType					autoindex_;
	std::string						path_;
	std::string						root_;
	std::vector<std::string>				index_;
	std::map< int, std::string >    error_page_;
	long long								client_max_body_size_;
	std::vector<std::string>	    allowed_methods_;
	std::map<int, std::string>		return_code_;
	bool                            	has_return_;
	bool					upload_allowed_;
	std::string				upload_location_;
	std::map<std::string, std::string>	cgi_;

};

std::ostream&	operator<<( std::ostream& out, const LocationConfig& l );

#endif
