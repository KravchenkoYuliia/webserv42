/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 18:24:00 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 13:16:07 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream> // ostringstream
#include "http/HttpConstants.hpp"
#include "http/HttpResponse.hpp"

HttpResponse::HttpResponse() {
	header_ = "";
	body_ = "";
}

HttpResponse::HttpResponse( const std::string& header, const std::string& body) {// delete if not used??
	header_ = header;
	body_ = body;
}

HttpResponse::~HttpResponse(void) {}

const std::string	HttpResponse::serialize() const {
	return header_ + body_;
}

//setters
//
void	HttpResponse::setHeader( const std::ostringstream& header ) {
	header_ = header.str();
}

void	HttpResponse::setBody( const std::string& body ) {
	body_ = body;
}


//getters
//
const std::string&	HttpResponse::getHeader() const {
	return header_;
}

const std::string&	HttpResponse::getBody() const {
	return body_;
}
