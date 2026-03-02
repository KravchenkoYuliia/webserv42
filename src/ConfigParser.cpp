#include "ConfigParser.hpp"

ConfigParser::ConfigParser( char* config_file )
	: lexer_(config_file) {

	mode_.push( MODE_GLOBAL );

	Token	token = lexer_.getNextToken();
	while ( token.getType() != TOKEN_ENDFILE ) {

		ConfigParser::parseTokens_( token );
		token = lexer_.getNextToken();
	}
	//
	//TODO delete visualisation function
	//
	ConfigParser::printAll();
}

void	ConfigParser::parseTokens_( Token& token ) {


	if ( token.getType() == TOKEN_LEFTBRACE ) {
		throw std::runtime_error( "Error in config: fix the braces");
	}

	else if ( token.getType() == TOKEN_RIGHTBRACE ) {
		ConfigParser::parseRightBrace_();
	}

	else if ( token.getType() == TOKEN_WORD ) {
		ConfigParser::parseWord_( token );
	}
}

void	ConfigParser::parseRightBrace_() {

	if ( mode_.top() == MODE_GLOBAL )
			throw std::runtime_error( "Error in config: fix the braces" );
	if ( mode_.size() > 1 ) {
		mode_.pop();
	}
}

void	ConfigParser::parseWord_( Token& token ) {


	const std::string&	current_word = token.getValue();

	if ( current_word == "server" ) {
		ConfigParser::parseWordServer_();
	}

	else if ( current_word == "location" ) {
		ConfigParser::parseWordLocation_();
	}

	else {
		ConfigParser::parseAnotherWord_( token );
	}


}

void	ConfigParser::parseWordServer_() {

	if ( mode_.top() != MODE_GLOBAL )
		throw std::runtime_error( "Error in config: server block is written wrong" );

	Token token = lexer_.getNextToken();
	if ( token.getType() != TOKEN_LEFTBRACE )
		throw std::runtime_error( "Error in config: server block must have braces: \"server {...}\"" );

	mode_.push( MODE_SERVER );
	ServerConfig	server_config;
	servers_list_.push_back( server_config );
}

void	ConfigParser::parseWordLocation_() {

	if ( mode_.top() != MODE_SERVER )
		throw std::runtime_error( "Error in config: location block is written wrong" );
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

void	ConfigParser::parseAnotherWord_( Token& token ) {

	if ( mode_.top() == MODE_GLOBAL ) {
		throw std::runtime_error( "Error in config: no data outside SERVER braces is allowed");
	}
	else if ( mode_.top() == MODE_SERVER ) {
		ConfigParser::parseWordInsideServerBloc_( token );
	}
	else if ( mode_.top() == MODE_LOCATION ){

	}
}

void	ConfigParser::parseWordInsideServerBloc_( Token& token ) {

	if ( token.getValue() == "listen" ) {
		ConfigParser::parseListenInsideServerBlock();
	}

	//TODO
	//if WORD is no one from the listed above -> error invalid input
}

void	ConfigParser::parseListenInsideServerBlock() {

	Token token = lexer_.getNextToken(); // this token must have port( numbers ); can also have interface
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


	//TODO
	//if WORD is no one from the listed above -> error invalid input
}

void	ConfigParser::parseListenInsideServerBlock() {

	Token token = lexer_.getNextToken(); // this token must be interface:port
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: listen require interface:port" );
	//
	// get interface from token
	//
	std::string::size_type	position = token.getValue().find(':');
	if ( position == token.getValue().npos )
		throw std::runtime_error( "Error in config: write \"interface:port\"" );
	ServerConfig&	current_server = servers_list_.back();
	current_server.setInterface( token.getValue().substr( 0, position ) );

=======
	//
	//get port from token
	//
	char* end;
	long port_long = std::strtol( portValue.c_str(), &end, 10 );
	if ( *end )
		throw std::runtime_error( "Error in config: port must be a number" );
	current_server.setPort( static_cast<uint16_t>(port_long) );


	token = lexer_.getNextToken(); // this token can be default_server
	if ( token.getType() != TOKEN_WORD )
		throw std::runtime_error( "Error in config: fix server block" );
	if ( token.getValue() == "default_server" ) {
		servers_list_.back().setDefaultServer( true );
	}
	else {
		ConfigParser::parseWord_( token );
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
			std::cout << std::endl << "	Location list: " << std::endl;
		for ( std::vector<LocationConfig>::size_type j = 0; j < servers_list_[i].getLocationList().size(); j++ ) {

			std::cout << "		Location[" << j << "] has path: "
				<< servers_list_[i].getLocationList()[j].getPath() << std::endl;
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

