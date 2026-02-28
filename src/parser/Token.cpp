#include "Token.hpp"

Token::Token()
	: type_(TOKEN_WORD),
	value_("default") {}

Token::Token( TokenType type, std::string value )
					: type_(type),
					  value_(value) {}

Token::Token(const Token& other) { *this = other; }

Token&	Token::operator=(const Token& other) {

	if (this != &other) {
		this->type_ = other.type_;
		this->value_ = other.value_;
	}
	return *this;
}

Token::~Token() {}


const TokenType&	Token::getType() const {
	return type_;
}

const std::string&	Token::getValue() const {
	return value_;
}
