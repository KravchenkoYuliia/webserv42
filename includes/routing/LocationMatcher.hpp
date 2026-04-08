#ifndef LOCATIONMATCHER_HPP
#define LOCATIONMATCHER_HPP

#include <iostream>
#include "ServerConfig.hpp"

class LocationMatcher {

public:
	static const LocationConfig	matchLocation( const ServerConfig& selected_server, const std::string& uri );
	static bool			isUriLongerThanLocation( const std::string& uri, const std::string& location );

private:
	LocationMatcher();
	LocationMatcher(const LocationMatcher& other);
	LocationMatcher&	operator = (const LocationMatcher& other);

	~LocationMatcher();

};

#endif
