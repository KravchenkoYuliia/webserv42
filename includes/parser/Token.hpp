#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum TokenType {
	TOKEN_WORD,
	TOKEN_LEFTBRACE,
	TOKEN_RIGHTBRACE,
	TOKEN_ENDFILE,
};

class Token {

public:
	Token(const Token& other);
	Token( TokenType type, std::string value );
	Token&	operator = (const Token& other);
	~Token();


	TokenType       	getType() const;
	const std::string&	getValue() const;

private:
	TokenType		type_;
	std::string		value_;

	Token();
};

#endif
