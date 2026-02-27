#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser( char* config_file ) {

	openBrace_ = 0;
	mode_.push_back( MODE_GLOBAL );
	waiting_for_brace_ = false;

	Lexer	lexer( config_file );

	Token	token = lexer.getNextToken();
	while ( token.getType() != TOKEN_ENDFILE ) {

		std::cout << "token is " << token.getValue() << std::endl << std::endl;
		ConfigParser::parseTokens_( token );
		token = lexer.getNextToken();
	}

	/*if ( openBrace_ != 0 ) {
		throw std::runtime_error( "Error: extra parenthesis in configuration file" );
	}*/
}

ConfigParser::ConfigParser(const ConfigParser& other) { *this = other; }
ConfigParser&	ConfigParser::operator=(const ConfigParser& other) {

	if (this != &other) {
		this->server_ = other.server_;
	}
	return	*this;
}

//
//TODO
//use as prototype to parseTokens
//
/*void	ConfigParser::parseLine_( std::string line ) {

	int	i = 0;
	int	count_server = 0;
	while ( std::isspace( line[i] ))
		i++;
	//
	//find "server " or "server{" to start filling class ServerConfig
	//
	if ( line.substr( i, 6 ) == "server" && ( line[i+6] == ' ' || line[i+6] == '{' )) {
		
		ServerConfig server;
		server_.push_back( server );
		server_[count_server].setPort(8080); 
		std::cout << "Server" << count_server << " has port " << server_[count_server].getPort() << std::endl;
		count_server += 1;
	}
	return ;
}*/

void	ConfigParser::parseTokens_( Token token ) {


	if ( token.getType() == TOKEN_LEFTBRACE ) {
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
	
	else if ( token.getType() == TOKEN_RIGHTBRACE ) {

		if ( mode_.back() == MODE_GLOBAL )
			throw std::runtime_error( "Error in config: fix the braces" );
		if ( mode_.size() > 1 ) {
			mode_.pop_back();
		}
	}

	else if ( token.getType() == TOKEN_WORD ) {
	
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
}

ConfigParser::~ConfigParser() {}
