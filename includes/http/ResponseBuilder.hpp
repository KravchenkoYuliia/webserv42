/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:46:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/18 12:28:49 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <map>
#include <sstream> //stringstream
#include "http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"
#include "ServerConfig.hpp"
#include "MergedConfig.hpp"

enum status { SUCCESS, ERROR };

class ResponseBuilder {
public:
	ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data );
    	~ResponseBuilder();

	const HttpResponse&	build();

private:
	HttpResponse	response_;
	MergedConfig	config_data_;

	int			code_;
	std::string		code_meaning_;
	std::ostringstream	header_;

//Build `Return` response **************************************************************************
	const HttpResponse&	buildReturnResponse( const std::map<int, std::string>& return_data );
	void			setFirstLineOfReturnResponse();
	void			buildReturnRedirection( const std::string& what_is_return );
	void			buildReturnPageHtml( const std::string what_is_return );
	void			buildBasicReturn( const std::string& what_is_return );
	std::string		getCodeMeaning();

//Check possible errors
	int	checkMethodInRequest( HttpRequest::Method current_method, const std::vector<std::string>& allowed_methods );
	int	checkBodySize( size_t current_body_size, size_t max_body_size );

//Build `Error` response
	void	buildErrorResponse( int code );
	void	addHeaderLineFor405Error();


//Utils ********************************************************************************************
	std::string		generateDefaultPage();
	std::string		readContentFromFile( const std::string& file );
	const std::string	buildPathFromRootAndFile( const std::string& file );


	ResponseBuilder();
	ResponseBuilder( const ResponseBuilder& copy );
	ResponseBuilder& operator=( const ResponseBuilder& copy );

};

#endif
