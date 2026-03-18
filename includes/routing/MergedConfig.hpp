#ifndef MERGEDCONFIG_HPP
#define MERGEDCONFIG_HPP

#include <iostream>
#include "ServerConfig.hpp"

class MergedConfig {

public:
	MergedConfig();
	MergedConfig( const ServerConfig& server, const LocationConfig& location );
	MergedConfig( const MergedConfig& other );
	MergedConfig&	operator = ( const MergedConfig& other );

	~MergedConfig();

	const std::map<int, std::string>&	getReturn() const;
	std::string				getRoot() const;
	const std::vector<std::string>&		getMethods() const;
	const std::map<int, std::string>&	getErrorPage() const;
	size_t					getMaxBodySize() const;
/*
	bool				has_return_ ;           +
	std::map<int, std::string>	return_code_ ;          +

	std::vector<std::string>	server_name_;
	std::string			root_;                  + 
	std::vector<std::string>	index_;
	std::map<int, std::string>	error_page_;            +
	AutoindexType			autoindex_;
	long long			client_max_body_size_;  +
	std::vector<std::string>	allowed_methods_;       +
	bool				upload_allowed_;
	std::string			upload_location_;
*/
private:

	ServerConfig	server_;
	LocationConfig	location_;
};

#endif
