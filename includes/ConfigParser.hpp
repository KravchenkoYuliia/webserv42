#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

class ServerConfig;
class Lexer;
class Token;

enum	current_mode {

	MODE_GLOBAL,
	MODE_SERVER,
	MODE_LOCATION
};

class ConfigParser {

public:
	ConfigParser( char* config_file );
	~ConfigParser();


private:
	void				parseTokens_( Token token );
	int				openBrace_;
	bool				waiting_for_brace_;
	
	std::vector<current_mode>	mode_;
	std::vector<ServerConfig>	server_;
	

	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );	
};

#endif
