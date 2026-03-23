/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:42:02 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/30 19:32:37 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>         // close
#include <sys/epoll.h>      // EPOLLIN , EPOLLOUT
#include <sys/socket.h>     // recv
#include <errno.h>          // errno, EAGAIN, EWOULDBLOCK
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
    selected_location_()
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
        {
            // TODO: remove check with EAGAIN and replace with a CTRL-C check through signal
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break ;
            reactor_.deleteHandler(fd_);
            return ;
        }
        else if (bytes_received > 0)
        {
            // TODO: remove printing lines when done
            std::cout.write(buffer, bytes_received);
            std::cout.flush();
            std::cout << std::endl;

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

// TODO: add check on bytes_send
// TODO: track progress across multiple EPOLLOUT events.
void ConnectionHandler::handleWrite()
{
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
            // TODO: update the logging error message or maybe throw an exception?
            std::cerr << "Error sending data!" << std::endl;
            reactor_.deleteHandler(fd_);
            return ;
        }
        bytes_sent_ += bytes;
    }

    // Done sending → close connection
    if (bytes_sent_ >= total_size)
    {
        /*
            TODO:
                - [x] reset bytes_sent_ to 0
                - [x] reset serialized_response_
                - [] reset request_parser_
                - [x] reset server_resolved_ to false
        */
        bytes_sent_ = 0;
        serialized_response_.clear();
        server_resolved_ = false;
        // setWantRead(true); // TODO: uncomment this line is setWantRead(false) is kept in handleWrite
        setWantWrite(false);
        // finish to write, update state from EPOLLOUT to EPOLLIN
        reactor_.updateHandler(this);
    }
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


void ConnectionHandler::prepareResponse(int status)
{
    MergedConfig config_data(selected_server_, selected_location_);
    ResponseBuilder builder(request_parser_.getRequest(), config_data, status);
    const HttpResponse& response = builder.getResponse();

    serialized_response_ = response.serialize();
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
