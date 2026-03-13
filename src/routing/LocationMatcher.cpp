#include "LocationMatcher.hpp"

const LocationConfig&	LocationMatcher::matchLocation( const ServerConfig& selected_server, const std::string& uri ) {

	std::string::size_type			count_match;
	std::string				biggest_match = "/";
	const std::vector<LocationConfig>&	locations = selected_server.getLocationList();
	int					index_of_matched_location = 0;

	for ( std::vector<LocationConfig>::size_type i = 0; i < locations.size(); i++ ) {

		count_match = 0;
		const std::string&	path = locations[i].getPath();
		if ( path == uri ) {
			return locations[i];
		}
		if ( uri.rfind( path, 0 ) == std::string::npos ) {
			continue;
		}
		if ( biggest_match.length() < path.length() ) {
			biggest_match = path;
			index_of_matched_location = i;
		}
	}

	return locations[index_of_matched_location];
}
