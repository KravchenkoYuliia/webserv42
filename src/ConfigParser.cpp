#include "ConfigParser.hpp"

ConfigParser::ConfigParser() {}
ConfigParser::ConfigParser( char* config_file ) {

	std::ifstream	config_stream( config_file );
	if ( !config_stream || config_stream.fail() )
		throw std::runtime_error("Error: can't read from configuration file");

	std::string	line;
	while ( std::getline( config_stream, line) ) {
		ConfigParser::createTokens_( line );
		ConfigParser::parseTokens_();
		//ConfigParser::parseLine_( line );
	}
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
void	ConfigParser::parseLine_( std::string line ) {

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
}


void	ConfigParser::createTokens_( std::string line ) {
	(void)line;
}

void	ConfigParser::parseTokens_() {

}

ConfigParser::~ConfigParser() {}
