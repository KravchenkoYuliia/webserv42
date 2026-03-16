/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:47:03 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/10 15:42:19 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <string>
#include <map>

class HttpResponse {
public:
	HttpResponse();
	HttpResponse( const std::string& header, const std::string& body);//delete if not used
	~HttpResponse();
	
	const std::string	serialize() const;
//setters
//
	void			setHeader( const std::ostringstream& header );
	void			setBody( const std::string& body );
//getters
//
	const std::string&	getHeader() const;
	const std::string&	getBody() const;
private:
	//int                                 status_code_value_;         // ex. 200
	// std::string                         status_code_description_;   // ex. OK

	std::string	header_;
	std::string	body_;


	HttpResponse( const HttpResponse& copy );
	HttpResponse& operator=( const HttpResponse& copy );
};

#endif
