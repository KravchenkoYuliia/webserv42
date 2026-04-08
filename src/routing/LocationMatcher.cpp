#include "LocationMatcher.hpp"

const LocationConfig	LocationMatcher::matchLocation( const ServerConfig& selected_server, const std::string& uri ) {

	std::string				biggest_match = "/";
	const std::vector<LocationConfig>&	locations = selected_server.getLocationList();
	int					index_of_matched_location = 0;

	for ( std::vector<LocationConfig>::size_type i = 0; i < locations.size(); i++ ) {

		const std::string&	path = locations[i].getPath();
		if ( path == uri ) {
			return locations[i];
		}
		if ( uri.rfind( path, 0 ) == std::string::npos ) {
			continue;
		}
        if ( isUriLongerThanLocation( uri, path ) ) {
            continue;
        }
		if ( biggest_match.length() < path.length() ) {
			biggest_match = path;
			index_of_matched_location = i;
		}
	}
	if ( index_of_matched_location == 0 && uri == "/" && locations[0].getPath() != "/" ) {
		return LocationConfig();
	}

	return locations[index_of_matched_location];
}


bool	LocationMatcher::isUriLongerThanLocation( const std::string& uri, const std::string& location ) {

	size_t i = 0;

	while ( i < location.length() ) {

		if ( location[i] != uri[i] )
			break;
		i++;
	}
	if ( i >= location.length() && i < uri.length() && ( (i != 0 && uri[i - 1] != '/') && uri[i] != '/' ) ) {
		return true;
	}

	return false;
}
