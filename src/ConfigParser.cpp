#include "ConfigParser.hpp"

ConfigParser::ConfigParser( char* config_file ) 
	: lexer_(config_file) {

	mode_.push_back( MODE_GLOBAL );
	waiting_for_brace_ = false;

	Token	token = lexer_.getNextToken();
	while ( token.getType() != TOKEN_ENDFILE ) {

		ConfigParser::parseTokens_( token );
		token = lexer_.getNextToken();
	}
}

void	ConfigParser::parseTokens_( Token token ) {


	if ( token.getType() == TOKEN_LEFTBRACE ) {
		ConfigParser::parseLeftBrace_();
	}

	else if ( token.getType() == TOKEN_RIGHTBRACE ) {
		ConfigParser::parseRightBrace_();
	}

	else if ( token.getType() == TOKEN_WORD ) {
		ConfigParser::parseWord_( token );	
	}
}

void	ConfigParser::parseLeftBrace_() {
	
	if ( mode_.back() == MODE_GLOBAL ) {
			
		if ( waiting_for_brace_ ) {

			mode_.push_back( MODE_SERVER );
			ServerConfig	server;
			servers_list_.push_back( server );
			waiting_for_brace_ = false;
		}
		else
			throw std::runtime_error( "Error in config: fix the braces");
	}
	else if ( mode_.back() == MODE_SERVER ) {
	
		if ( waiting_for_brace_ ) {

			mode_.push_back( MODE_LOCATION );
			//adding location instance in latest Server in server_
			waiting_for_brace_ = false;
		}
		else
			throw std::runtime_error( "Error in config: fix the braces");
	}
}


void	ConfigParser::parseRightBrace_() {

	if ( mode_.back() == MODE_GLOBAL )
			throw std::runtime_error( "Error in config: fix the braces" );
	if ( mode_.size() > 1 ) {
		mode_.pop_back();
	}
}

void	ConfigParser::parseWord_( Token token ) {

	if ( token.getValue() == "server" ) {
		
		if ( mode_.back() != MODE_GLOBAL )
			throw std::runtime_error( "Error in config: server block is written wrong" );
		
		waiting_for_brace_ = true;
	}
	else if ( token.getValue() == "server{" ) {

		if ( mode_.back() != MODE_GLOBAL )
			throw std::runtime_error( "Error in config: server block is written wrong" );
		mode_.push_back( MODE_SERVER );
		ServerConfig	server;
		servers_list_.push_back( server );

	}

	else if ( token.getValue() == "location" ) {
	
		if ( mode_.back() != MODE_SERVER )
			throw std::runtime_error( "Error in config: location block is written wrong" );
		
		waiting_for_brace_ = true;
	}
	else {
		ConfigParser::storeValueOfWord( token );
	}


}

void	ConfigParser::storeValueOfWord( Token token ) {

	if ( mode_.back() == MODE_GLOBAL ) {
		throw std::runtime_error( "Error in config: no data outside SERVER braces is allowed");
	}
	else if ( mode_.back() == MODE_SERVER ) {
		//std::cout << "SERVER MODE: " << token.getValue() << std::endl;
		/*if ( waiting_for_brace_ == true ) {
			servers_list_[ servers_list_.size()-1 ]. 
		}*/
		ConfigParser::addWordToServer( token );
	}
	else if ( mode_.back() == MODE_LOCATION ){

		//std::cout << "LOCATION MODE: " << token.getValue() << std::endl;
	}
}

void	ConfigParser::addWordToServer( Token token ) {

	if ( token.getValue() == "listen" ) {
		token = lexer_.getNextToken();
		if ( token.getType() == TOKEN_ENDFILE )
			throw std::runtime_error( "Error in config: listen require interface:port" );
		// get interface
		std::string::size_type	position = token.getValue().find(':');
		if ( position == token.getValue().npos )
			throw std::runtime_error( "Error in config: write \"interface:port\"" );
		servers_list_[ servers_list_.size()-1 ].setInterface( token.getValue().substr( 0, position ) );
		//get port
		char* end;
		long port_long = std::strtol( token.getValue().substr( position + 1 ).c_str(), &end, 10 );
		if ( *end )
			throw std::runtime_error( "Error in config: port must be a number" );
		servers_list_[ servers_list_.size()-1 ].setPort( static_cast<uint16_t>(port_long) );
		std::cout << "Server " << servers_list_.size()-1 << " has interface[" << servers_list_[ servers_list_.size()-1 ].getInterface() << "] and port[" << servers_list_[ servers_list_.size()-1 ].getPort() << "]" << std::endl;
	}
}


ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { *this = other; }
ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {}
	return	*this;
}
ConfigParser::~ConfigParser() {}

