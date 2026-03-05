#include "ConfigParser.hpp"
#include <algorithm>
#include <iostream>
#include <limits>
ConfigParser::ConfigParser( char* config_file )
	: lexer_(config_file) {

	mode_.push( MODE_GLOBAL );

	Token	token = lexer_.getNextToken();
	while ( token.getType() != TOKEN_ENDFILE ) {

		ConfigParser::parseTokens( token );
		token = lexer_.getNextToken();
	}

	ConfigParser::fillEmptyDirectives();



	//TODO delete visualisation function
	//
	ConfigParser::printAll();
}

void	ConfigParser::parseTokens( const Token& token ) {

	if ( token.getType() == TOKEN_LEFTBRACE ) {
		throw std::runtime_error( "Error in config: unexpected '{' outside server or location block");
	}

	else if ( token.getType() == TOKEN_RIGHTBRACE ) {
		ConfigParser::parseRightBrace();
	}

	else if ( token.getType() == TOKEN_WORD ) {
		ConfigParser::parseDirectiveWord( token );
	}
	else if ( token.getType() == TOKEN_SEMICOLON ) {
		//TODO
		//does it useless this condition or semicilon only can be after key words like listen ip:port;
	}
}

void	ConfigParser::parseRightBrace() {

	if ( mode_.top() == MODE_GLOBAL )
			throw std::runtime_error( "Error in config: fix the braces" );
	if ( mode_.size() > 1 ) {
		mode_.pop();
	}
}

void	ConfigParser::parseDirectiveWord( const Token& token ) {


	const std::string&	current_word = token.getValue();

	if ( current_word == "server" ) {
		ConfigParser::parseWordServer();
	}

	else if ( current_word == "location" ) {
		ConfigParser::parseWordLocation();
	}

	else {
		ConfigParser::parseWords( token );
	}
}

void	ConfigParser::parseWordServer() {

	if ( mode_.top() != MODE_GLOBAL )
		throw std::runtime_error( "Error in config: server block must be declared at the top level" );

	Token token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_LEFTBRACE )
		throw std::runtime_error( "Error in config: server block must have braces: \"server {...}\"" );

	mode_.push( MODE_SERVER );
	ServerConfig	server_config;
	servers_list_.push_back( server_config );
}

void	ConfigParser::parseWordLocation() {

	if ( mode_.top() != MODE_SERVER )
		throw std::runtime_error( "Error in config: location block must be declared inside server block" );
	//
	//nextToken must be a location's path :
	//
	Token token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: location block must have path: \"location /PATH {}\"" );
	if ( token.getValue()[0] != '/' )
		throw std::runtime_error( "Error in config: location path must start with a slash: \"location /PATH {}\"" );

	//
	//check if current path is unique
	const std::vector<LocationConfig>&	loc_list = servers_list_.back().getLocationList();
	for ( std::vector<LocationConfig>::size_type i = 0; i < loc_list.size(); i++ ) {
		if ( loc_list[i].getPath() == token.getValue() )
			throw std::runtime_error( "Error in config: same location can't be repeated twice for the same server" );
	}

	//
	//adding an instance of class Location with /path to location_list in lattest server
	//
	mode_.push( MODE_LOCATION );
	LocationConfig	location_config( token.getValue() );
	if ( servers_list_.size() < 1 )
		throw std::runtime_error( "Error in config: location block is outside of server block" );

	servers_list_.back().setLocationList( location_config );

	//
	//check if there is open brace after location /path
	//
	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_LEFTBRACE )
		throw std::runtime_error( "Error in config: location block must have braces: \"location /PATH {}\"" );
}

void	ConfigParser::parseWords( const Token& token ) {

	if ( mode_.top() == MODE_GLOBAL ) {
		throw std::runtime_error( "Error in config: no data outside SERVER braces is allowed");
	}
	else if ( mode_.top() == MODE_SERVER ) {
		ConfigParser::parseWordInServer( token );
	}
	else if ( mode_.top() == MODE_LOCATION ){
		ConfigParser::parseWordInLocation( token );

	}
}

void	ConfigParser::parseWordInServer(const Token& token ) {

	if ( token.getValue() == "listen" ) {
		ConfigParser::parseListenInServer();
	}
	else if ( token.getValue() == "server_name" ) {
		ConfigParser::parseServerNameInServer();
	}
	else if ( token.getValue() == "root" ) {
		ConfigParser::parseRootInServer();
	}
	else if ( token.getValue() == "index" ) {
		ConfigParser::parseIndexInServer();
	}
	else if ( token.getValue() == "error_page" ) {
		ConfigParser::parseErrorPageInServer();
	}
	else if ( token.getValue() == "autoindex" ) {
		ConfigParser::parseAutoindexInServer();
	}
	else if ( token.getValue() == "client_max_body_size" ) {
		ConfigParser::parseMaxBodyInServer();
	}


	//TODO
	//if WORD is no one from the listed above -> error invalid input
}

void	ConfigParser::parseListenInServer() {

	Token token = lexer_.getNextToken(); // this token must have port( numbers ) and can also have interface
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: listen require port or interface:port" );
	std::string	portValue;
	ServerConfig&	current_server = servers_list_.back();
	//
	// get interface from token if there is any
	//


	std::string::size_type	position = token.getValue().find(':');
	if ( position == 0 || position == token.getValue().size() - 1 )
		throw std::runtime_error( "Error in config: listen to invalid port" );
	if ( position != token.getValue().npos ) {
		current_server.setInterface( token.getValue().substr( 0, position ) );
		portValue = token.getValue().substr( position + 1 );
	}
	else {
		portValue = token.getValue();
	}

	//
	//get port from token
	//
	char* end;
	long port_long = std::strtol( portValue.c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: port must be a number" );
	int	first_valid_port = 1;
	int	last_valid_port = 65535;
	if ( port_long < first_valid_port || port_long > last_valid_port )
		throw std::runtime_error( "Error in config: invalid port number. Must be in range 1-65535" );

	current_server.setPort( static_cast<uint16_t>(port_long) );


	token = lexer_.getNextToken(); // this token can be default_server or  ;
	if ( token.getType() == TOKEN_SEMICOLON )
		return ;
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix server block" );

	if ( token.getValue() == "default_server" ) {
		servers_list_.back().setDefaultServer( true );
	}
	else {
		ConfigParser::parseDirectiveWord( token );
	}
}

void	ConfigParser::parseServerNameInServer() {

	Token	token = lexer_.getNextToken();
	while ( token.getType() == TOKEN_WORD ) {
		servers_list_.back().setServerName( token.getValue() );
		token = lexer_.getNextToken();
	}
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix server_name block");
}

void	ConfigParser::parseRootInServer() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix root block");

	servers_list_.back().setRoot( token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix root block");
}

void	ConfigParser::parseIndexInServer() {

	Token	token = lexer_.getNextToken();
	while ( token.getType() == TOKEN_WORD ) {
		servers_list_.back().setIndex( token.getValue() );
		token = lexer_.getNextToken();
	}

	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix index block");
}

void	ConfigParser::parseErrorPageInServer() {

	//next token must be error number like 404
	//
	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block");

	char* end;
	long error_nb = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: error_page needs a number of the error" );
	int	first_valid_error = 100;
	int	last_valid_error = 599;
	if ( error_nb < first_valid_error || error_nb > last_valid_error )
		throw std::runtime_error( "Error in config: invalid error number" );

	//next token must be a page error
	//
	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block - error page is missing");

	servers_list_.back().setErrorPage( error_nb, token.getValue() );


	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix error_page block - semicolon is missing");
}

void	ConfigParser::parseAutoindexInServer() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix autoindex block");
	if ( token.getValue() == "on" )
		servers_list_.back().setAutoindex( AUTOINDEX_ON );
	else if ( token.getValue() == "off" )
		servers_list_.back().setAutoindex( AUTOINDEX_OFF );
	else
		throw std::runtime_error( "Error in config: fix autoindex block - only on/off are allowed");

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix autoindex block - semicolon is missing");
}

void	ConfigParser::parseMaxBodyInServer() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix max_body block");

	for ( std::string::size_type i = 0; i < token.getValue().size(); i++ ) {
		if ( !isdigit(  token.getValue()[0] ) )
			throw std::runtime_error( "Error in config: max_body must be a number > 0 and < INT_MAX" );
		while ( isdigit( token.getValue()[i] ) )
			i++;
		if ( !isdigit( token.getValue()[i] ) ) {
			if ( token.getValue()[i + 1] )
				throw std::runtime_error( "Error in config: max_body can be a number only with m/M, k/K, g/G" );
		}
	}

	char* end;
	long max_body = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( max_body < 1 || max_body > std::numeric_limits<int>::max() )
		throw std::runtime_error( "Error in config: max_body must be a number > 0 and < INT_MAX" );
	unsigned long client_max_body_size = max_body;
	if ( *end ) {
		char	letter = *end;
		if ( letter == 'k' || letter == 'K' )
			client_max_body_size *= 1024;
		else if ( letter == 'm' || letter == 'M' ) {
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
		}
		else if ( letter == 'g' || letter == 'G' ) {
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
		}
	}

	servers_list_.back().setClientMaxBodySize( client_max_body_size );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix max_body block - semicolon is missing");
}

void	ConfigParser::parseWordInLocation( const Token& token ) {

	if ( token.getValue() == "root" ) {
		ConfigParser::parseRootInLocation();
	}
	else if ( token.getValue() == "index" ) {
		ConfigParser::parseIndexInLocation();
	}
	else if ( token.getValue() == "error_page" ) {
		ConfigParser::parseErrorPageInLocation();
	}
	else if ( token.getValue() == "autoindex" ) {
		ConfigParser::parseAutoindexInLocation();
	}
	else if ( token.getValue() == "client_max_body_size" ) {
		ConfigParser::parseMaxBodyInLocation();
	}


	//TODO
	//if WORD is no one from the listed above -> error invalid input

}

void	ConfigParser::parseRootInLocation() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix root block in location");

	servers_list_.back().getLocationList().back().setRoot( token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix root block in location");
}

void	ConfigParser::parseIndexInLocation() {

	Token	token = lexer_.getNextToken();
	while ( token.getType() == TOKEN_WORD ) {
		servers_list_.back().getLocationList().back().setIndex( token.getValue() );
		token = lexer_.getNextToken();
	}

	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix index block in location");
}

void	ConfigParser::parseErrorPageInLocation() {

	//next token must be error number like 404
	//
	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block in location" );

	char* end;
	long error_nb = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: error_page in location needs a number of the error" );
	int	first_valid_error = 100;
	int	last_valid_error = 599;
	if ( error_nb < first_valid_error || error_nb > last_valid_error )
		throw std::runtime_error( "Error in config: invalid error number" );

	//next token must be a page error
	//
	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block in location - error page is missing");

	servers_list_.back().getLocationList().back().setErrorPage( error_nb, token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix error_page block - semicolon is missing");
}

void	ConfigParser::parseAutoindexInLocation() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix autoindex block in location");
	if ( token.getValue() == "on" )
		servers_list_.back().getLocationList().back().setAutoindex( AUTOINDEX_ON );
	else if ( token.getValue() == "off" )
		servers_list_.back().getLocationList().back().setAutoindex( AUTOINDEX_OFF );
	else
		throw std::runtime_error( "Error in config: fix autoindex block in location- only on/off are allowed");

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix autoindex block in location- semicolon is missing");
}

void	ConfigParser::parseMaxBodyInLocation() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix max_body block in location");

	for ( std::string::size_type i = 0; i < token.getValue().size(); i++ ) {
		if ( !isdigit(  token.getValue()[0] ) )
			throw std::runtime_error( "Error in config: max_body in location must be a number > 0 and < INT_MAX" );
		while ( isdigit( token.getValue()[i] ) )
			i++;
		if ( !isdigit( token.getValue()[i] ) ) {
			if ( token.getValue()[i + 1] )
				throw std::runtime_error( "Error in config: max_body in location can be a number only with m/M, k/K, g/G" );
		}
	}

	char* end;
	long max_body = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( max_body < 1 || max_body > std::numeric_limits<int>::max() )
		throw std::runtime_error( "Error in config: max_body in location must be a number > 0 and < INT_MAX" );
	unsigned long client_max_body_size = max_body;
	if ( *end ) {
		char	letter = *end;
		if ( letter == 'k' || letter == 'K' )
			client_max_body_size *= 1024;
		else if ( letter == 'm' || letter == 'M' ) {
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
		}
		else if ( letter == 'g' || letter == 'G' ) {
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
			client_max_body_size *= 1024;
		}
	}

	servers_list_.back().getLocationList().back().setClientMaxBodySize( client_max_body_size );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix max_body in location block - semicolon is missing");
}

void	ConfigParser::fillEmptyDirectives() {
	//
	//check if there is server block that has 0 location
	//
	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		if ( servers_list_[i].getLocationList().empty() ) {
			LocationConfig	location_config;
			servers_list_[i].setLocationList( location_config );
		}
	}
	//check if there is server block that has no index
	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		if ( servers_list_[i].getIndex().empty() ) {
			servers_list_[i].setIndex( kDefaultIndex );
		}
	}
	//
	//check if there is server block that has 0 error page

	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		if ( servers_list_[i].getErrorPage().empty() ) {
			servers_list_[i].setErrorPage( 404, "404.html" ); //TODO create a list of default error pages
		}
	}
}

//
//TODO delete visualisation function
//
void	ConfigParser::printAll() {

	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		std::cout << "Server[" << i << "]" << "-> "<< &servers_list_[i] << " has: " << std::endl
			<< "	Port: " << servers_list_[i].getPort() << std::endl
			<< "	Interface: " << servers_list_[i].getInterface();
			if ( servers_list_[i].getDefaultServer() == true )
				std::cout << " default_server";
			std::cout << std::endl << "	Server name: ";
			for ( std::vector<std::string>::size_type s = 0; s < servers_list_[i].getServerName().size(); s++ ) {
				std::cout << servers_list_[i].getServerName()[s] << " ";
			}
			std::cout << std::endl << "	Root: " << servers_list_[i].getRoot() << std::endl
				<< "	Index: ";
			for ( std::vector<std::string>::size_type in = 0; in < servers_list_[i].getIndex().size(); in++ ) {
				std::cout << servers_list_[i].getIndex()[in] << " ";
			}
			std::cout << std::endl << "	Error page: " << std::endl;
			for ( std::map<int, std::string>::const_iterator it = servers_list_[i].getErrorPage().begin(); it != servers_list_[i].getErrorPage().end(); it++ ) {
				std::cout << "		" << it->first << " ---> " << it->second << std::endl;
			}
			std::cout << "	Autoindex: ";
			if ( servers_list_[i].getAutoindex() == true )
				std::cout << "on" << std::endl;
			else
				std::cout << "off" << std::endl;
			std::cout << "	Client_max_body_size: " << servers_list_[i].getClientMaxBodySize() << std::endl;

//------------------------------------------------------------------------------------------------------------------------------
			std::cout << "	Location list: " << std::endl;
		for ( std::vector<LocationConfig>::size_type j = 0; j < servers_list_[i].getLocationList().size(); j++ ) {

			std::cout << "		Location[" << j << "] has:" << std::endl << "				path: "
				<< servers_list_[i].getLocationList()[j].getPath() << std::endl;
				if ( servers_list_[i].getLocationList()[j].getRoot() != "" )
					std::cout << "				root: " << servers_list_[i].getLocationList()[j].getRoot() << std::endl;
				if ( !servers_list_[i].getLocationList()[j].getIndex().empty() ) {
					std::cout << "				index: ";
					for ( std::vector<std::string>::size_type ind = 0; ind < servers_list_[i].getLocationList()[j].getIndex().size(); ind++ ) {
						std::cout << servers_list_[i].getLocationList()[j].getIndex()[ind] << " ";
					}
					std::cout << std::endl;
				}
				if ( !servers_list_[i].getLocationList()[j].getErrorPage().empty() ) {
					std::cout << "				error page: " << std::endl;
					for ( std::map<int, std::string>::const_iterator it = servers_list_[i].getLocationList()[j].getErrorPage().begin(); it != servers_list_[i].getLocationList()[j].getErrorPage().end(); it++ ) {
						std::cout << "					" << it->first << " ---> " << it->second << std::endl;
					}
				}
				std::cout << "				autoindex: ";
				if ( servers_list_[i].getLocationList()[j].getAutoindex() == true )
					std::cout << "on" << std::endl;
				else if ( servers_list_[i].getLocationList()[j].getAutoindex() == false )
					std::cout << "off" << std::endl;
				else
					std::cout << std::endl;
				if ( servers_list_[i].getLocationList()[j].getClientMaxBodySize() != 0)
					std::cout << "				client_max_body_size: " << servers_list_[i].getLocationList()[j].getClientMaxBodySize() << std::endl;

		}
	}
}




ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { *this = other; }
ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {}
	return	*this;
}
ConfigParser::~ConfigParser() {}

