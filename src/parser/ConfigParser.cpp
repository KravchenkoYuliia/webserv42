
#include "ConfigParser.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <iostream>
#include <limits>

ConfigParser::ConfigParser( char* config_file )
	: lexer_(config_file) {

	mode_.push( MODE_GLOBAL );

	Token	token = lexer_.getNextToken();
    if ( token.getType() == TOKEN_ENDFILE )
		throw std::runtime_error( "Error in config: empty file" );

	while ( token.getType() != TOKEN_ENDFILE ) {

		ConfigParser::parseTokens( token );
		token = lexer_.getNextToken();
	}

    if ( servers_list_.empty() )
		throw std::runtime_error( "Error in config: empty file" );

	ConfigParser::fillEmptyDirectives();
	ConfigParser::checkUpload();
}

std::vector<ServerConfig>&	ConfigParser::getServers() {
	return servers_list_;
}

std::map< uint16_t, std::vector<ServerConfig> >& ConfigParser::getMapOfPortWithServers() {

	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		map_[servers_list_[i].getPort()].push_back( servers_list_[i] );
	}

	return map_;
}

void	ConfigParser::parseTokens( const Token& token ) {

	if ( token.getType() == TOKEN_WORD ) {
		ConfigParser::parseDirectiveWord( token );
	}
	else if ( token.getType() == TOKEN_RIGHTBRACE ) {
		ConfigParser::parseRightBrace();
	}
	else if ( token.getType() == TOKEN_LEFTBRACE ) {
		throw std::runtime_error( "Error in config: unexpected '{'");
	}
	else if ( token.getType() == TOKEN_SEMICOLON ) {
		throw std::runtime_error( "Error in config: unexpected ';'");
	}
	else if ( token.getType() == TOKEN_NEWLINE ) {
		return ;
	}
}

void	ConfigParser::parseRightBrace() {

	if ( mode_.top() == MODE_GLOBAL )
			throw std::runtime_error( "Error in config: unexpected '}'" );
	if ( mode_.size() > 1 ) {
		mode_.pop();
	}
}

void	ConfigParser::parseDirectiveWord( const Token& token ) {


	const std::string&	current_word = token.getValue();

	if ( current_word == "server" ) {
		ConfigParser::parseDirectiveServer();
	}
	else if ( current_word == "location" ) {
		ConfigParser::parseDirectiveLocation();
	}
	else {
		ConfigParser::parseWords( token );
	}
}

void	ConfigParser::parseDirectiveServer() {

	if ( mode_.top() != MODE_GLOBAL )
		throw std::runtime_error( "Error in config: server block must be declared at the top level" );

	Token token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_LEFTBRACE )
		throw std::runtime_error( "Error in config: server block must have braces: \"server {...}\"" );

	mode_.push( MODE_SERVER );
	ServerConfig	server_config;
	servers_list_.push_back( server_config );
}

void	ConfigParser::parseDirectiveLocation() {

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


	if ( token.getValue() == "listen" ) {  //
		ConfigParser::parseListenInServer();// only server's directive
	}
	else if ( token.getValue() == "server_name" ) { //
		ConfigParser::parseServerNameInServer();// only server's directive
	}
	else if ( token.getValue() == "root" ) {
		ConfigParser::parseRoot();
	}
	else if ( token.getValue() == "index" ) {
		ConfigParser::parseIndex();
	}
	else if ( token.getValue() == "error_page" ) {
		ConfigParser::parseErrorPage();
	}
	else if ( token.getValue() == "autoindex" ) {
		ConfigParser::parseAutoindex();
	}
	else if ( token.getValue() == "client_max_body_size" ) {
		ConfigParser::parseClientMaxBodySize();
	}
	else if ( token.getValue() == "return" ) {
		ConfigParser::parseReturn();
	}
	else if ( token.getValue() == "allowed_methods" ) { //
		ConfigParser::parseAllowedMethodsInLocation();//only location's directive
	}
	else if ( token.getValue() == "upload_allowed" ) { //
		ConfigParser::parseUploadAllowedInLocation();//only location's directive
	}
	else if ( token.getValue() == "upload_location" ) { //
		ConfigParser::parseUploadLocationInLocation();//only location's directive
	}
	else {
		throw std::runtime_error( "Error in config: invalid directive");
	}

}

void	ConfigParser::parseListenInServer() {

    if ( mode_.top() != MODE_SERVER )
		throw std::runtime_error( "Error in config: directive `listen` only is possible inside server block" );

	if ( servers_list_.back().getHasListen() == true )
		throw std::runtime_error( "Error in config: only one `listen` is allowed in the same server" );
	servers_list_.back().setHasListen();
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

		std::string	interface = token.getValue().substr( 0, position );
		if ( interface != "127.0.0.1" && interface != "localhost")
			throw std::runtime_error( "Error in config: listen to invalid interface" );

		current_server.setInterface( interface );
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
		//check if there is no server with the same port that is also `default_server`

		for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
			if ( servers_list_[i].getPort() == port_long ) {
				if ( servers_list_[i].getDefaultServer() == true )
					throw std::runtime_error( "Error in config: servers with the same port only can have 1 default_server");
			}
		}

		servers_list_.back().setDefaultServer( true );
		token = lexer_.getNextToken(); //must be semicolon
		if ( token.getType() == TOKEN_SEMICOLON )
			return ;
		else
			throw std::runtime_error( "Error in config: fix listen block - semicolon is missing" );
	}
	else {
		throw std::runtime_error( "Error in config: invalid word after port" );
		//ConfigParser::parseDirectiveWord( token );
	}
}

void	ConfigParser::parseServerNameInServer() {

    if ( mode_.top() != MODE_SERVER )
		throw std::runtime_error( "Error in config: directive `server_name` only is possible inside server block" );

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix server_name block - directive does not provide any value");
	while ( token.getType() == TOKEN_WORD ) {
		servers_list_.back().setServerName( token.getValue() );
		token = lexer_.getNextToken();
	}
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix server_name block - semicolon is missing");
}

void	ConfigParser::parseIndex() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix index block - directive does not provide any value");
	while ( token.getType() == TOKEN_WORD ) {
		if ( mode_.top() == MODE_SERVER )
			servers_list_.back().setIndex( token.getValue() );
		else if ( mode_.top() == MODE_LOCATION )
			servers_list_.back().getLocationList().back().setIndex( token.getValue() );
		token = lexer_.getNextToken();
	}

	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix index block - semicolon is missing");
}

void	ConfigParser::parseRoot() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix root block - directive does not provide any value");

	if ( mode_.top() == MODE_SERVER )
		servers_list_.back().setRoot( token.getValue() );
	else if ( mode_.top() == MODE_LOCATION )
		servers_list_.back().getLocationList().back().setRoot( token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix root block - semicolon is missing");
}

void	ConfigParser::parseErrorPage() {

	//next token must be error number like 404
	//
	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block - directive does not provide any value");

	char* end;
	long error_nb = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: error_page needs a number of the error" );

	int			numbers[] = { 400, 403, 404, 405, 413, 415, 500, 502, 504, 505 };
	std::vector<int>	implemented_codes( numbers, numbers + sizeof( numbers )/sizeof(numbers[0]) );
	if ( std::find( implemented_codes.begin(), implemented_codes.end(), error_nb ) == implemented_codes.end() )
		throw std::runtime_error( "Error in config: invalid or not implemented error number" );

	//next token must be a page error
	//
	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix error_page block - error page is missing");

    if ( mode_.top() == MODE_SERVER )
	    servers_list_.back().setErrorPage( error_nb, token.getValue() );
    else if ( mode_.top() == MODE_LOCATION )
	    servers_list_.back().getLocationList().back().setErrorPage( error_nb, token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix error_page block - semicolon is missing");
}

void	ConfigParser::parseAutoindex() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix autoindex block - directive does not provide any value");
	if ( token.getValue() == "on" ) {
        if ( mode_.top() == MODE_SERVER )
		    servers_list_.back().setAutoindex( AUTOINDEX_ON );
        else if ( mode_.top() == MODE_LOCATION )
    	    servers_list_.back().getLocationList().back().setAutoindex( AUTOINDEX_ON );
    }
	else if ( token.getValue() == "off" ) {
        if ( mode_.top() == MODE_SERVER )
		    servers_list_.back().setAutoindex( AUTOINDEX_OFF );
        else if ( mode_.top() == MODE_LOCATION )
    	    servers_list_.back().getLocationList().back().setAutoindex( AUTOINDEX_OFF );
    }
	else
		throw std::runtime_error( "Error in config: fix autoindex block - only on/off are allowed");

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix autoindex block - semicolon is missing");
}

void	ConfigParser::parseClientMaxBodySize() {

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix client_max_body_size block - directive does not provide any value");

	for ( std::string::size_type i = 0; i < token.getValue().size(); i++ ) {
		if ( !isdigit(  token.getValue()[0] ) )
			throw std::runtime_error( "Error in config: client_max_body_size must be a number > 0 and < INT_MAX" );
		while ( isdigit( token.getValue()[i] ) )
			i++;
		if ( token.getValue()[i] && !isdigit( token.getValue()[i] ) ) {
			if ( token.getValue()[i + 1] )
				throw std::runtime_error( "Error in config: client_max_body_size can be a number only with m/M, k/K, g/G" );
		}
	}

	char* end;
	long max_body = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( max_body < 1 || max_body > std::numeric_limits<int>::max() )
		throw std::runtime_error( "Error in config: client_max_body_size max_body must be a number > 0 and < INT_MAX" );
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
        else
    		throw std::runtime_error( "Error in config: fix client_max_body_size block - invalid input");

	}
    if ( mode_.top() == MODE_SERVER )
    	servers_list_.back().setClientMaxBodySize( client_max_body_size );
    else if ( mode_.top() == MODE_LOCATION )
        servers_list_.back().getLocationList().back().setClientMaxBodySize( client_max_body_size );

    token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix client_max_body_size block - semicolon is missing");
}

void	ConfigParser::parseReturn() {

    ConfigParser::checkIfOnlyOneReturn();

	//next token must be a number like 200
	//
	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix return block - directive does not provide any return code");

	char* end;
	long code = std::strtol( token.getValue().c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: fix return block - must have a return code");

	int numbers[] = { 200, 201, 204, 301, 302, 303, 307, 308, 400, 403, 404, 405, 413, 415, 500, 502, 504, 505 };
	std::vector<int> 	implemented_codes( numbers, numbers + sizeof( numbers )/sizeof(numbers[0]) );
	if ( std::find( implemented_codes.begin(), implemented_codes.end(), code ) == implemented_codes.end() )
		throw std::runtime_error( "Error in config: invalid or not implemented code after return" );

	//next token can be a page / text / nothing
	//
	token = lexer_.getNextToken();
	if ( token.getType() == TOKEN_SEMICOLON ) {
		ConfigParser::setReturn( code, "" );
		return ;
	}
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix return block");

	ConfigParser::setReturn( code, token.getValue() );

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix return block - semicolon is missing");
}

void	ConfigParser::setReturn( int code, std::string value ) {

	if ( mode_.top() == MODE_SERVER )
	        servers_list_.back().setReturn( code, value );
	else if ( mode_.top() == MODE_LOCATION )
        	servers_list_.back().getLocationList().back().setReturn( code, value );
}

void	ConfigParser::checkIfOnlyOneReturn() {

	if ( mode_.top() == MODE_SERVER ) {
		if ( servers_list_.back().getHasReturn() == true )
			throw std::runtime_error( "Error in config: only one `return` is allowed in the same block" );
        servers_list_.back().setHasReturn();
	}
	else if ( mode_.top() == MODE_LOCATION ) {
		if ( servers_list_.back().getLocationList().back().getHasReturn() == true )
			throw std::runtime_error( "Error in config: only one `return` is allowed in the same block" );
        servers_list_.back().getLocationList().back().setHasReturn();
	}
}

void	ConfigParser::parseAllowedMethodsInLocation() {

	if ( mode_.top() != MODE_LOCATION )
		throw std::runtime_error( "Error in config: directive `allowed_methods` only is possible inside location block" );

    int count_methods = 0;
	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix allowed_methods block in location - directive does not provide any value " );
	while ( token.getType() == TOKEN_WORD ) {

		if ( token.getValue() != "GET" && token.getValue() != "POST" && token.getValue() != "DELETE" )
			throw std::runtime_error( "Error in config: fix allowed_methods block - invalid method");

		count_methods += 1;
		if ( count_methods > 3 )
			throw std::runtime_error( "Error in config: fix allowed_methods block - maximum 3 methods are allowed (GET, POST, DELETE)");

		servers_list_.back().getLocationList().back().setAllowedMethods( token.getValue() );
		token = lexer_.getNextToken();
	}

	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix allowed_methods block - semicolon is missing");

}

void	ConfigParser::parseUploadAllowedInLocation() {

	if ( mode_.top() != MODE_LOCATION )
		throw std::runtime_error( "Error in config: directive `upload_allowed` only is possible inside location block" );

	Token	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix upload_allowed block - directive does not provide any value");
	
	if ( token.getValue() == "on" )
		servers_list_.back().getLocationList().back().setUploadAllowed();
	else if ( token.getValue() != "off" )
		throw std::runtime_error( "Error in config: fix upload_allowed block - only \"on\" \"off\" are possible");

	token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_SEMICOLON )
		throw std::runtime_error( "Error in config: fix upload_allowed block - semicolon is missing");
}

void	ConfigParser::parseUploadLocationInLocation() {
	
	if ( mode_.top() != MODE_LOCATION )
		throw std::runtime_error( "Error in config: directive `upload_location` only is possible inside location block" );
	if ( servers_list_.back().getLocationList().back().getUploadAllowed() == true ) {

		Token	token = lexer_.getNextToken();
		if ( token.getType() != TOKEN_WORD )
			throw std::runtime_error( "Error in config: fix upload_location block - directive does not provide any value");
		servers_list_.back().getLocationList().back().setUploadLocation( token.getValue() );

		token = lexer_.getNextToken();
		if ( token.getType() != TOKEN_SEMICOLON )
			throw std::runtime_error( "Error in config: fix upload_location block - semicolon is missing");
	}
	else { 
		Token	token = lexer_.getNextToken();
		std::cout << "Token after upload_location " << token.getValue() << std::endl;
		if ( token.getType() == TOKEN_WORD ) {
			std::cout << "Token after word after upload_location is " << token.getValue() << std::endl;
			token = lexer_.getNextToken();
		}
	}	
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
			servers_list_[i].setErrorPage( 400, "400.html" );
			servers_list_[i].setErrorPage( 403, "403.html" );
			servers_list_[i].setErrorPage( 404, "404.html" );
			servers_list_[i].setErrorPage( 405, "405.html" );
			servers_list_[i].setErrorPage( 413, "413.html" );
			servers_list_[i].setErrorPage( 500, "500.html" );
		}
	}
	//allowed methods
	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		for ( std::vector<LocationConfig>::size_type j = 0; j < servers_list_[i].getLocationList().size(); j++ ) {
			if ( servers_list_[i].getLocationList()[j].getAllowedMethods().empty() ) {
 				servers_list_[i].getLocationList()[j].setAllowedMethods( "GET" );
 				servers_list_[i].getLocationList()[j].setAllowedMethods( "POST" );
 				servers_list_[i].getLocationList()[j].setAllowedMethods( "DELETE" );
			}
		}
	}
}

void	ConfigParser::checkUpload() {

	for ( std::vector<ServerConfig>::size_type i = 0; i < servers_list_.size(); i++ ) {
		for ( std::vector<LocationConfig>::size_type j = 0; j < servers_list_[i].getLocationList().size(); j++ ) {

			if ( servers_list_[i].getLocationList()[j].getUploadAllowed() == true 
					&& servers_list_[i].getLocationList()[j].getUploadLocation() == "" ) {
				throw std::runtime_error( "Error in config: upload_allowed is ON but no upload_location is provided");
			}
		}
	}
}

std::ostream&	operator<<( std::ostream& out, std::map<uint16_t, std::vector<ServerConfig> >& m ) {

	out << "--------------------------------------------------" << std::endl
		<< "\e[1;92m" <<  "Map< port, vector of Server > :" << std::endl;
	for ( std::map<uint16_t, std::vector<ServerConfig> >::iterator it = m.begin(); it != m.end(); it++ ) {
		out << "\e[1;92m" << "Port [" << it->first << "] is in " << it->second.size() << " servers" << std::endl << "\033[0m";
		
		for ( std::vector<ServerConfig>::size_type i = 0; i < it->second.size(); i++ ) {
			out << "\t" << "\e[4;37m" << "Server["<< i << "] has" << std::endl
				<< it->second[i];
		}
		out << "\033[0m";
	}
	out << "-------------------------------------------------";

	return out;
}


ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { *this = other; }
ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {}
	return	*this;
}
ConfigParser::~ConfigParser() {}

