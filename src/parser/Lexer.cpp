#include "Lexer.hpp"

Lexer::Lexer() {}


Lexer::Lexer( char* config_file) {

	Lexer::checkExtension( config_file );
	stream_.open( config_file );
	if ( stream_.fail() || !stream_.is_open() )
		throw std::runtime_error( "Error: can't open configuration file" );
}

Token	Lexer::getNextToken() {

	current_ = Lexer::getCharWithoutWhitespaces();
	if ( current_ == ' ' )
		return Token( TOKEN_ENDFILE, "" );

	current_ = Lexer::parseComment();
	if ( current_ == ' ' )
		return Token( TOKEN_ENDFILE, "" );

	if ( current_ == '{' ) {
		return Token( TOKEN_LEFTBRACE, "{" );
	}
	else if ( current_ == '}' ) {
		return Token( TOKEN_RIGHTBRACE, "}" );
	}
	else if ( current_ == ';' ) {
		return Token( TOKEN_SEMICOLON, ";" );
	}
	else if  ( current_ == '\n' ) {
		return Token( TOKEN_NEWLINE, "\n" );
	}
	else {

		std::string	word = Lexer::parseWord();

		if ( word == "" )
			return Token( TOKEN_ENDFILE, "" );
		else
			return Token( TOKEN_WORD, word );
	}

	return Token( TOKEN_ENDFILE, "" );
}

std::string	Lexer::parseWord() {

	int char_size = 1;
	std::string	word( char_size, current_ );
	while ( !std::isspace( current_ ) && current_ != '{' ) {

		int getReturn = stream_.get();
		if ( getReturn == -1 )
			return "";

        current_ = static_cast<char>( getReturn );
		if ( std::isspace( current_ ) || current_ == '{' || current_ == ';' ) {
			stream_.unget();
			if ( stream_.fail() ) {
				throw std::runtime_error( "Error in config: ifstream fail during unget() call" );
			}
			break;
		}
		word += current_;
	}

	return word;
}

char    Lexer::parseComment() {

        while ( current_ == '#' ) {

		std::string	line;
		getline( stream_, line );

		current_ = Lexer::getCharWithoutWhitespaces();
		if ( current_ == ' ' )
			return ' ';
	}
	return current_;
}

char    Lexer::getCharWithoutWhitespaces() {

    int	getReturn;
    current_ = ' ';
	while ( current_ == ' ' || current_ == '\t' ) { //std::isspace( current_ ) ) {

		getReturn = stream_.get();
		if ( getReturn == -1 )
			return ' ';
		current_ = static_cast<char>( getReturn );
	}

    return current_;
}
void	Lexer::checkExtension( std::string config_file ) {

	std::string	extention = ".conf";
	if ( config_file.size() < extention.size() + 1 ||
		config_file.find(".conf") == config_file.npos ||
			config_file.substr( config_file.size() - 5, 5 ) != extention )
		throw std::runtime_error( "Error in config: wrong extension of config file");
}

Lexer::Lexer(const Lexer& other) { *this = other; }

Lexer&	Lexer::operator=(const Lexer& other) {

	if (this != &other) {}
	return *this;
}

Lexer::~Lexer() {}
