#ifndef LEXER_HPP
#define LEXER_HPP

#include <fstream>
#include "Token.hpp"

class Lexer {

public:
	Lexer();
	Lexer( char* config_file);
	~Lexer();

	Token	getNextToken();

private:
	std::ifstream	stream_;
	char		current_;

	void	checkExtension( std::string config_file );

	char		getCharWithoutWhitespaces();
	char    	parseComment();
	std::string	parseWord();

	Lexer( const Lexer& other );
	Lexer&	operator = ( const Lexer& other );
};

#endif
