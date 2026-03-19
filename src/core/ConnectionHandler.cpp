/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yukravch <yukravch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:42:02 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/24 13:21:55 by yukravch         ###   ########.fr       */
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
#include "utils/Utils.hpp"
#include "ServerMatcher.hpp"
#include "LocationMatcher.hpp"
#include "MergedConfig.hpp"

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
    port_(port),
    servers_(servers)
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
    // TODO: remove this log
    std::cout << "EPOLLIN case" << std::endl;

    // read the request until recv == 0
    while (true)
    {
        ssize_t bytes_received = recv(fd_, buffer, sizeof(buffer), 0);
        if (bytes_received == 0)
        {
            reactor_.deleteHandler(fd_);
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

            // save bytes into a read_buffer
            request_parser_.appendData(buffer, bytes_received);

            while (true)
            {
                RequestParser::ResultType result = request_parser_.parseNext();
                if (result == RequestParser::ParserResult::ERROR)
                {
                    // TODO: remove this log
                    std::cout << "result == RequestParser::ParserResult::ERROR bloc" << std::endl;

                    reactor_.deleteHandler(fd_);
                    return ;
                }
                else if (result == RequestParser::ParserResult::AGAIN)
                {
                    // TODO: remove this log
                    std::cout << "result == RequestParser::ParserResult::AGAIN bloc" << std::endl;
                    break;
                }
                else if (result == RequestParser::ParserResult::OK)
                {
                    // TODO: remove this log
                    std::cout << "result == RequestParser::ParserResult::OK bloc" << std::endl;
                    if (request_parser_.isComplete())
                    {
                        // TODO: remove this log
                        std::cout << "request_.isComplete bloc" << std::endl;
                        // TODO: integrate the server, host like the following in the response_builder object
                        const ServerConfig&        selected_server = ServerMatcher::matchServer(servers_, request_parser_.getHeader(Http::Headers::HOST), port_);
                        const LocationConfig&      selected_location = LocationMatcher::matchLocation(selected_server, request_parser_.getUri() );
                        MergedConfig        config_data( selected_server, selected_location );

                        ResponseBuilder     builder( request_parser_.getRequest(), config_data );
                        const HttpResponse&        response = builder.build();

                        serialized_response_ = response.serialize();

                        bytes_sent_ = 0;
                        setWantWrite(true);
                        // setWantRead(false); // TODO: should i disable the EPOLLIN here?
                        reactor_.updateHandler(this);
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
    // TODO these logs
    std::cout << "EPOLLOUT case" << std::endl;

    if (serialized_response_.empty())
        return ;
    while (bytes_sent_ < static_cast<int>(serialized_response_.length()))
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
    if (bytes_sent_ >= static_cast<int>(serialized_response_.length()))
    {
        /*
            TODO:
                - [x] reset bytes_sent_ to 0
                - [] reset serialized_response_
                - [] reset request_parser_
        */
        bytes_sent_ = 0;
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
