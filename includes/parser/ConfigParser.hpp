#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <vector>
#include <stack>
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

class ServerConfig;
class LocationConfig;
class Lexer;
class Token;

enum	ParserMode {
	MODE_GLOBAL,
	MODE_SERVER,
	MODE_LOCATION
};

class ConfigParser {

public:
	ConfigParser( char* config_file );
	~ConfigParser();


private:

	std::stack<ParserMode>	    mode_;
	std::vector<ServerConfig>	servers_list_;
	Lexer						lexer_;

	void	parseTokens( const Token& token );
	void	parseRightBrace();
	void	parseDirectiveWord( const Token& token );
	void	parseWordServer();
	void	parseWordLocation();
	void	parseWords( const Token& token );
	void	fillEmptyDirectives();

	void	parseWordInServer( const Token& token );
	void	parseListenInServer();
	void	parseServerNameInServer();
	void	parseRootInServer();
	void	parseIndexInServer();
	void	parseErrorPageInServer();

	void	parseWordInLocation( const Token& token );
	void	parseRootInLocation();
	void	parseIndexInLocation();
	void	parseErrorPageInLocation();
	void	parseAutoindexInServer();

	//
	//TODO remove this function
	//
	void	printAll();

	ConfigParser();
	ConfigParser( const ConfigParser& other );
	ConfigParser&	operator = ( const ConfigParser& other );
};

#endif
