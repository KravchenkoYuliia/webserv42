#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <algorithm>
#include <vector>
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

class ServerConfig;
class LocationConfig;
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

	std::stack<current_mode>	mode_;
	std::vector<ServerConfig>	servers_list_;
	Lexer						lexer_;


	void				parseTokens_( Token& token );
	void				parseRightBrace_();
	void				parseWord_( Token& token );
	void				parseWordServer_( const std::string& word );
	void				parseWordLocation_();
	void				parseAnotherWord_( Token& token );
	void				parseWordInsideServerBloc_( Token& token );
	void				parseListenInsideServerBlock();

	//
	//TODO remove this function
	//
	void	printAll();
	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );
};

#endif
