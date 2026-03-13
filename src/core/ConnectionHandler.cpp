/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionHandler.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 16:42:02 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:19:32 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <unistd.h>         // close
#include <sys/epoll.h>      // EPOLLIN , EPOLLOUT
#include <sys/socket.h>     // recv
#include <errno.h>          // errno, EAGAIN, EWOULDBLOCK
#include "core/ConnectionHandler.hpp"
#include "http/ResponseBuilder.hpp"
#include "utils/Utils.hpp"

// ############################# ConnectionHandler Class #############################

// ------------------------- Destructor / Constructor -------------------------

ConnectionHandler::ConnectionHandler(int client_fd, Reactor& reactor)
    :   fd_(client_fd),
        reactor_(reactor),
        request_parser_()
{
    // TODO: delete this log
    std::cout << "ConnectionHandler default constructor called" << std::endl;
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

/*
    if EPOLLIN:
        readFromClient()
        if requestComplete():
            handleRequest()
            switch fd to EPOLLOUT

    if EPOLLOUT:
        if sendToClient() == false:
            close connection
*/

/*
READ PHASE

1. recv in loop until EAGAIN
2. append to read_buffer_
3. check if "\r\n\r\n" exists
    if not → return
4. if headers not parsed:
    parse first line (method, path, version)
    parse Content-Length
5. if method == GET:
    request complete
6. if method == POST:
    check read_buffer_.size() >= header_end + 4 + content_length_
    if not → return
7. build response
8. switch to EPOLLOUT
*/

/*
EPOLLIN
   ↓
recv() until EAGAIN
   ↓
append to request buffer
   ↓
parse until parser needs more data
   ↓
if request complete → switch to EPOLLOUT
*/

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
                        ResponseBuilder     builder;
                        HttpResponse        response = builder.build(request_parser_.getRequest());

                        serialized_response_ = response.serialize();
                        bytes_sent_ = 0;
                        reactor_.updateHandler(this, EPOLLOUT | EPOLLET); // TODO: not sure about EPOLLET
                        return ;
                    }
                }
            }
        }
    }

    /*
        check if request fully received
        1. search presence of "\r\n\r\n" in the read_buffer that indicate the end of headers and save it into a variable called header_end
        2. check if header_end == std::string::npos, which indicate if we reach the end of the string or not
           if yes => return
           if no => continue
        3. is headers already parsed?
           if no, then parse hearders
             - 1st line : HTTP/1.1, method
             - Content-Length
             - others headers
           if yes, then check request method:
               if method GET => return than the request is complete
               if method POST => check that (header_end  + 4 (length of the word POST) + request.content_length) <= read_buffer.size()
    */
   /*
        Build response

        if request complete then build response
        1. Set headers according to request.method
        ???
        10. save the response into variabel write_buffer
   */
}

// TODO: add check on bytes_send
// TODO: track progress across multiple EPOLLOUT events.

/*
WRITE PHASE STEPS

1. If state != WRITING_RESPONSE → return
2. Call send() using remaining bytes only
3. If send > 0:

   * Increase bytes_sent_
4. If send == -1:

   * If EAGAIN → return (wait for next EPOLLOUT)
   * Else → close connection
5. If bytes_sent_ == write_buffer_.size():

   * Response fully sent
   * If keep-alive:

     * reset connection state
     * switch to EPOLLIN
   * Else:

     * close connection
 */
void ConnectionHandler::handleWrite()
{
    // TODO these logs
    std::cout << "EPOLLOUT case" << std::endl;

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
        else if (bytes > 0)
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

        // finish to write, update state from EPOLLOUT to EPOLLIN
        reactor_.updateHandler(this, EPOLLIN | EPOLLET);
    }
    reactor_.deleteHandler(fd_);
}

void ConnectionHandler::handleError()
{
    reactor_.deleteHandler(fd_);

}
