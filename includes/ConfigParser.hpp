#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <algorithm>
#include <vector>
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
	std::vector<ServerConfig>	servers_list_;
	Lexer						lexer_;


	void				parseTokens_( Token token );
	void				parseLeftBrace_();
	void				parseRightBrace_();
	void				parseWord_( Token token );
	void				storeValueOfWord( Token token );
	void				addWordToServer( Token token );

	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );
};

#endif
