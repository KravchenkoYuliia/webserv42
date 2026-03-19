/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:46:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/24 13:37:34 by yukravch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_BUILDER_HPP
#define RESPONSE_BUILDER_HPP

#include <map>
#include <sstream> //stringstream
#include <dirent.h> //opendir
#include "http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"
#include "ServerConfig.hpp"
#include "MergedConfig.hpp"

enum status { SUCCESS, ERROR, NOT_SPECIFIED=-1, IS_FILE, IS_DIR };

class ResponseBuilder {
public:

	ResponseBuilder( const HttpRequest& request, const MergedConfig& config_data );
    ~ResponseBuilder();

	const HttpResponse&	build();

private:
	HttpResponse	response_;
	MergedConfig	config_data_;

	int			code_;
	bool			error_;
	int			file_or_dir_;
	std::string		code_meaning_;
	std::ostringstream	header_;

	void	setFirstLineOfHeader();
	void	setResponse( const HttpRequest& request, const MergedConfig& config_data );
	void	setLastLineOfHeader();

//Build `Return` response **************************************************************************
	void			buildDirectReturn( const std::map<int, std::string>& return_data );
	void			setReturnRedirection( const std::string& what_is_return );
	void			handleResource( const std::string what_is_return );
	void			setContentType( const std::string& path );
	void			setBasicReturn( const std::string& what_is_return );
	std::string		getCodeMeaning();

//Check possible errors
	int	checkMethodInRequest( HttpRequest::Method current_method, const std::vector<std::string>& allowed_methods );
	int	checkBodySize( size_t current_body_size, size_t max_body_size );

//Build `Error` response
	void	buildErrorResponse( int code );
	void	addHeaderLineFor405Error();
	void	setErrorPageHtml( const std::string page_from_config );


//Build Classic Response
	void	buildResponseAccordingToMethod( const HttpRequest& request );
	void	buildResponseGET( const HttpRequest& request );
	int	handleFile( const std::string& path );
	void	handleDirectory( const std::string& path );
	void	handleAutoindex( const std::string& path );
	void	buildResponsePOST( const HttpRequest& request );
	void	buildResponseDELETE( const HttpRequest& request );
	void	buildListing( std::vector<std::string>& files_from_dir, const std::string& path );

//Utils ********************************************************************************************
	std::string		    generateDefaultPage();
	std::string		    readContentFromFile( const std::string& file );
	const std::string	buildPathFromRootAndResource( const std::string file );
	void			setErrorState( int error_code );
	int			checkFileExistence( const char* path );
	int			checkFilePermissions( const char* path );
	int			checkIfPathExists( const std::string& path );
	const std::string	getExtension( const std::string& path );
	const std::string		cut_prefix_from_uri( const std::string& uri_from_request );
	DIR*				openDirectory( const std::string& path );
	void				readDirectory( DIR* dir_ptr, std::vector<std::string>& files_from_dir );

	ResponseBuilder();
	ResponseBuilder( const ResponseBuilder& copy );
	ResponseBuilder& operator=( const ResponseBuilder& copy );

};

#endif
