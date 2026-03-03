#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <map>
#include <string>

#define kDefaultLoctaionPath "/"
class LocationConfig {

public:

	LocationConfig();
	LocationConfig( const std::string& path );
	LocationConfig(const LocationConfig& other);
	LocationConfig&	operator = (const LocationConfig& other);
	~LocationConfig();

//setters

	void	setRoot( std::string root );

//getters

	const std::string&	getPath() const;
	const std::string&	getRoot() const;

private:
//	bool							autoindex_;
	std::string						path_;
	std::string						root_;
/*	std::string						index_;
	std::map< int, std::string >	error_page_;
	int								client_max_body_size_;
	std::vector<std::string>		methods_;
	std::map<int, std::string>		return_page_;
*/

};

#endif
