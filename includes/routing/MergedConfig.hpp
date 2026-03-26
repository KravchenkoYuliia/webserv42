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
	const std::string&			getPath() const;
	const std::vector<std::string>&		getIndex() const;
	AutoindexType				getAutoindex() const;
	bool					getUploadAllowed() const;
	const std::string&			getUploadLocation() const;

private:

	ServerConfig	server_;
	LocationConfig	location_;
};

#endif
