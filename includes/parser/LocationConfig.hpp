#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <map>
#include <string>
#include <vector>

#define kDefaultLoctaionPath "/"

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
	void	setClientMaxBodySize( unsigned long client_max_body_size );

//getters

	const std::string&		getPath() const;
	const std::string&		getRoot() const;
	const std::vector<std::string>&	getIndex() const;
	const std::map<int, std::string>&	getErrorPage() const;
	AutoindexType					getAutoindex() const;
	unsigned long			getClientMaxBodySize() const;


private:
	AutoindexType					autoindex_;
	std::string						path_;
	std::string						root_;
	std::vector<std::string>				index_;
	std::map< int, std::string >	error_page_;
	unsigned long								client_max_body_size_;
/*	std::vector<std::string>		methods_;
	std::map<int, std::string>		return_page_;
*/

};

#endif
