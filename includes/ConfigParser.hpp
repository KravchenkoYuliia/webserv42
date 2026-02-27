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
	bool				waiting_for_brace_;
	
	std::vector<current_mode>	mode_;
	std::vector<ServerConfig>	server_;
	

	void				parseTokens_( Token token );
	void				parseLeftBrace_();
	void				parseRightBrace_();
	void				parseWord_( Token token );

	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );	
};

#endif
