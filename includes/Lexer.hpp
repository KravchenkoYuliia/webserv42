#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include "Token.hpp"

class Lexer {

public:
	Lexer( char* file);
	~Lexer();

	Token	getNextToken();

private:
	std::ifstream	stream_;
	char		current_;
	
	Lexer();
	Lexer( const Lexer& other );
	Lexer&	operator = ( const Lexer& other );
};

#endif
