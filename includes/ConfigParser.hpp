#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <list> 
#include "webserv.hpp"
#include "ServerConfig.hpp"

class ServerConfig;
class ConfigParser {

public:
	ConfigParser();
	ConfigParser( char* config_file );
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );

	
	~ConfigParser();


private:
	void				parseLine_( std::string line );
	void				createTokens_( std::string line );
	void				parseTokens_();

	std::list<std::string>		tokens_;
	std::vector<ServerConfig>	server_;
};

#endif
