/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:42:02 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 18:10:18 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>         // close
#include <sys/epoll.h>      // EPOLLIN , EPOLLOUT
#include <sys/socket.h>     // recv
#include "core/CgiHandler.hpp"
#include "core/ConnectionHandler.hpp"
#include "http/HttpConstants.hpp"
#include "http/ResponseBuilder.hpp"
#include "routing/MergedConfig.hpp"
#include "utils/Utils.hpp"

// ############################# ConnectionHandler Class #############################

// ------------------------- Destructor / Constructor -------------------------

ConnectionHandler::ConnectionHandler(int client_fd,
    Reactor& reactor,
    uint16_t port,
    const std::vector<ServerConfig>& servers
):  BaseEventHandler(BaseEventHandler::CONNECTION),
    fd_(client_fd),
    reactor_(reactor),
    request_parser_(),
    bytes_sent_(0),
    port_(port),
    servers_(servers),
    server_resolved_(false),
    selected_server_(),
    selected_location_(),
    cgi_pending_(false)
{
}

ConnectionHandler::~ConnectionHandler(void)
{
    if (fd_ != -1)
        ::close(fd_);
}

// --------------------------- Public Getter Methods ---------------------------

int ConnectionHandler::getFd() const
{
    return (fd_);
}

// --------------------------- Public Member Methods ---------------------------

void ConnectionHandler::handleRead()
{
    char buffer[8192];

    while (true)
    {
        ssize_t bytes_received = recv(fd_, buffer, sizeof(buffer), 0);
        if (bytes_received == 0)
        {
            if (!request_parser_.isComplete())
            {
                std::cerr << "[ConnectionHandler::handleRead][ERROR] Client closed before full body received" << std::endl;
                prepareResponse(400);
            }
            return ;
        }
        else if (bytes_received < 0)
            break; // assume EAGAIN
        else if (bytes_received > 0)
        {
            // TODO: remove printing lines when done
            std::cout.write(buffer, bytes_received);
            std::cout.flush();

            // save bytes into a read_buffer
            request_parser_.appendData(buffer, bytes_received);

            while (true)
            {
                RequestParser::ResultType result = request_parser_.parseNext();
                if (result == RequestParser::ParserResult::ERROR)
                {
                    if (!server_resolved_)
                        resolveServerAndLocation();
                    prepareResponse(request_parser_.getErrorCode());
                    return ;
                }
                if (!server_resolved_)
                {
                    RequestParser::ParserState::Type state = request_parser_.getState();
                    if (state == RequestParser::ParserState::BODY_CONTENT_LENGTH
                        || state == RequestParser::ParserState::BODY_CHUNKED
                        || state == RequestParser::ParserState::BODY_NONE
                        || state == RequestParser::ParserState::COMPLETE)
                    {
                        server_resolved_ = true;
                        resolveServerAndLocation();
                        if (!checkIsMethodAllowed())
                            return;
                        if (!checkBodySizeLimit())
                            return;
                    }
                }
                if (result == RequestParser::ParserResult::AGAIN)
                    break;
                if (result == RequestParser::ParserResult::OK)
                {
                    if (request_parser_.isComplete())
                    {
                        prepareResponse(Http::Response::REQUEST_VALID);
                        return ;
                    }
                }
            }
        }
    }
}

void ConnectionHandler::handleWrite()
{
    if (serialized_response_.empty() && cgi_pending_)
    {
        handleCgi();
        return;
    }
    if (serialized_response_.empty())
        return ;
    size_t total_size = serialized_response_.size();
    while (bytes_sent_ < total_size)
    {
        ssize_t bytes = send(
            fd_,
            serialized_response_.data() + bytes_sent_,
            serialized_response_.size() - bytes_sent_,
            0 );
        if (bytes == -1)
        {
            std::cerr << "[ConnectionHandler::handleWrite] bytes send failed\n" << std::endl;
            reactor_.deleteHandler(fd_);
            return ;
        }
        if (bytes > 0)
        {
            bytes_sent_ += static_cast<size_t>(bytes);
        }
        else
        {
            // assume EAGAIN
            setWantWrite(true);
            reactor_.updateHandler(this);
            return;
        }
    }

    // Done sending → close connection
    bytes_sent_ = 0;
    serialized_response_.clear();
    cgi_output_buffer_.clear();
    request_parser_.reset();
    server_resolved_ = false;
    setWantWrite(false);
    reactor_.updateHandler(this);
    reactor_.deleteHandler(fd_);
}

void ConnectionHandler::handleError()
{
    reactor_.deleteHandler(fd_);
}

// --------------------------- Private Member Methods --------------------------

void ConnectionHandler::resolveServerAndLocation()
{
    selected_server_ = ServerMatcher::matchServer(
        servers_,
        request_parser_.getHeaderValue(Http::Headers::HOST)
    );

    selected_location_ = LocationMatcher::matchLocation(
        selected_server_,
        request_parser_.getUri()
    );
}


void ConnectionHandler::prepareResponse(size_t status)
{
    MergedConfig            config_data(selected_server_, selected_location_);
    size_t                  status_code = static_cast<size_t>(status);
    const HttpRequest&      request = request_parser_.getRequest();

    if (status_code == Http::Response::REQUEST_VALID && request.isCgiRequest(config_data.getCgi())) {
        cgi_output_buffer_.clear();
        cgi_pending_ = true;

        CgiHandler* cgi = new CgiHandler( reactor_, request, config_data, cgi_output_buffer_ ,fd_ );
        if (!cgi->execCgi())
        {
            delete cgi;
            status_code = 502; // Bad Gateway
        }
        setWantRead(false);
        setWantWrite(false);
        reactor_.updateHandler(this);
        return;
    }
    ResponseBuilder builder(request, config_data, status);
    serialized_response_ = builder.getResponse().serialize();

    bytes_sent_ = 0;
    setWantWrite(true);
    setWantRead(false);
    reactor_.updateHandler(this);
}

bool ConnectionHandler::checkBodySizeLimit()
{
    const std::string& content_length_str = request_parser_.getHeaderValue(Http::Headers::CONTENT_LENGTH);
    if (content_length_str.empty())
        return (true);

    bool success;
    long long content_length_value = Utils::parseLongLong(content_length_str, success, 10);

    long long selected_client_max_body_size = selected_location_.getClientMaxBodySize();
    if (selected_client_max_body_size == -1)
        selected_client_max_body_size = selected_server_.getClientMaxBodySize();
    if (!success || content_length_value > selected_client_max_body_size)
    {
        prepareResponse(413);
        return (false);
    }
    return (true);
}

bool ConnectionHandler::checkIsMethodAllowed()
{
    const std::string current_method = request_parser_.getRequest().getMethodToString();
    const std::vector<std::string> allowed_methods = selected_location_.getAllowedMethods();

    for ( std::vector<std::string>::const_iterator it = allowed_methods.begin();
            it != allowed_methods.end(); ++it )
    {
        if (*it == current_method)
            return (true);
    }
    prepareResponse(405);
    return (false);
}

void ConnectionHandler::handleCgi()
{
    cgi_pending_ = false;
    MergedConfig config_data(selected_server_, selected_location_);

    if (cgi_output_buffer_.empty())
    {
        std::cerr << "[ConnectionHandler::handleWrite] cgi_output_buffer_ is empty! ResponseBuilder will build the 502 response"<< std::endl;
        ResponseBuilder builder(request_parser_.getRequest(), config_data, 502);
        serialized_response_ = builder.getResponse().serialize();
    }
    else
    {
        ResponseBuilder builder(request_parser_.getRequest(),
                                config_data,
                                cgi_output_buffer_);
        serialized_response_ = builder.getResponse().serialize();
    }
    bytes_sent_ = 0;
    setWantWrite(true);
    reactor_.updateHandler(this);
}
