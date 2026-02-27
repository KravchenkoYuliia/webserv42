#include "Lexer.hpp"

Lexer::Lexer() {}


Lexer::Lexer( char* file) {
	
	stream_.open( file );
	if ( stream_.fail() || !stream_.is_open() )
		throw std::runtime_error( "Error: can't open configuration file" );
}

Token	Lexer::getNextToken() {

	int	getReturn;
	current_ = ' ';
	while ( std::isspace( current_ ) ) {
	
		getReturn = stream_.get();
		if ( getReturn == -1 )
			return Token( TOKEN_ENDFILE, "" );

		current_ = static_cast<char>( getReturn );
	}

	while ( current_ == '#' ) {
		std::string	line;
		getline( stream_, line );
		
		current_ = ' ';
		while ( std::isspace( current_ ) ) {
			getReturn = stream_.get();
			if ( getReturn == -1 )
				return Token( TOKEN_ENDFILE, "" );

			current_ = static_cast<char>( getReturn );
		}
	}

	if ( current_ == '{' ) {
		return Token( TOKEN_LEFTBRACE, "{" );
	}
	else if ( current_ == '}' ) {
		return Token( TOKEN_RIGHTBRACE, "}" );
	}

	else {
		std::string	word( 1, current_ );
		while ( !std::isspace( current_ ) ) {
			
			getReturn = stream_.get();
			if ( getReturn == -1 )
				return Token( TOKEN_ENDFILE, "" );
			current_ = static_cast<char>( getReturn );
			if ( !std::isspace( current_ ) )
				word += current_;
		}
		return Token( TOKEN_WORD, word );
	}

	return Token( TOKEN_ENDFILE, "" );
}


Lexer::Lexer(const Lexer& other) { *this = other; }

Lexer&	Lexer::operator=(const Lexer& other) {

	if (this != &other) {}
	return *this;
}

Lexer::~Lexer() {}
