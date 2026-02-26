#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

class ServerConfig;
class Lexer;
class Token;

class ConfigParser {

public:
	ConfigParser( char* config_file );
	~ConfigParser();


private:
	void				parseTokens_( Token token );
	int				openBrace_;
	std::vector<ServerConfig>	server_;
	

	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );	
};

#endif
