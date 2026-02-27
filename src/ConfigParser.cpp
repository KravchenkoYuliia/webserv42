#include "ConfigParser.hpp"

ConfigParser::ConfigParser( char* config_file ) {

	mode_.push_back( MODE_GLOBAL );
	waiting_for_brace_ = false;

	Lexer	lexer( config_file );

	Token	token = lexer.getNextToken();
	while ( token.getType() != TOKEN_ENDFILE ) {

		std::cout << "token is " << token.getValue() << std::endl << std::endl;
		ConfigParser::parseTokens_( token );
		token = lexer.getNextToken();
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
			server_.push_back( server );
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
	if ( token.getValue() == "location" ) {
	
		if ( mode_.back() != MODE_SERVER )
			throw std::runtime_error( "Error in config: location block is written wrong" );
		
		waiting_for_brace_ = true;
	}
}


ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser(const ConfigParser& other) { *this = other; }
ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {}
	return	*this;
}
ConfigParser::~ConfigParser() {}

