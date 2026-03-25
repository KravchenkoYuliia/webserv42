/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseBuilder.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:46:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/25 13:02:06 by yukravch         ###   ########.fr       */
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

	const HttpResponse&	getResponse();

private:
	HttpResponse	response_;
	MergedConfig	config_data_;

	int			        code_;
	bool		    	error_;
	int			        file_or_dir_;
	std::string		    uri_;
	std::string		    code_meaning_;
	std::ostringstream	header_;

	void	        initialize_values( const std::string& uri, const MergedConfig& config_data );
	void	        setFirstLineOfHeader();
	void	        setLastLineOfHeader();
	void	        setContentType( const std::string& path );
	void		setServerAndDate();
	std::string		getCodeMeaning();

//Build `Return` response **************************************************************************
	void	buildReturn( const std::map<int, std::string>& return_data );
	void	buildRedirectionReturn( const std::string& what_is_return );
	void    buildBasicReturn( const std::string& what_is_return );

//Check possible errors
	int	    checkMethodInRequest( HttpRequest::Method current_method, const std::vector<std::string>& allowed_methods );
	int	    checkBodySize( size_t current_body_size, size_t max_body_size );
	int	    checkFileExistence( const char* path );
	int	    checkFilePermissions( const char* path );
	int	    checkIfPathExists( const std::string& path );

    void    setErrorState( int error_code );

//Build `Error` response
	void	buildErrorResponse( int code );
	void	setHeaderLineFor405Error();
	void	setErrorPageHtml( const std::string page_from_config );

//Build Classic Response
	void	buildResponseAccordingToMethod( const HttpRequest& request );
	void	buildResponseGET();
	void	handleUri( const std::string what_is_return );
	int		handleFile( const std::string& path );
	void	handleDirectory( const std::string& path );
	void	handleAutoindex( const std::string& path );
	void	buildResponsePOST( const HttpRequest& request );
	void	buildResponseDELETE( const HttpRequest& request );
	void	buildListing( std::vector<std::string>& files_from_dir );

//Utils ********************************************************************************************
	const std::string   generateDefaultPage();
	const std::string   readContentFromFile( const std::string& file );
	const std::string	buildPathFromRootAndResource( std::string root, std::string resource );
	const std::string	getExtension( const std::string& path );
	const std::string	cutPrefixFromUri( const std::string& uri_from_request );
	DIR*				openDirectory( const std::string& path );
	void				readDirectory( DIR* dir_ptr, std::vector<std::string>& files_from_dir );

	ResponseBuilder();
	ResponseBuilder( const ResponseBuilder& copy );
	ResponseBuilder& operator=( const ResponseBuilder& copy );

};

#endif
