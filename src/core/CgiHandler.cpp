/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 12:53:28 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/02 21:54:03 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>
#include <sstream>
#include <unistd.h>     // fork()
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid()
#include <signal.h>     // kill(), signal constants
#include <errno.h>      // errno
#include <cstdlib>      // std::exit()
#include <sys/epoll.h>  // EPOLLIN, EPOLLOUT, EPOLL_CTL_MOD
#include "core/CgiHandler.hpp"
#include "http/HttpConstants.hpp"
#include "utils/Utils.hpp"

// ############################# ConnectionHandler Class #############################

// ------------------------- Destructor / Constructor -------------------------

CgiHandler::CgiHandler(
        Reactor& reactor,
        const HttpRequest& request,
        const MergedConfig& merge_config,
        std::string& output_buffer,
        int fd
)   :   BaseEventHandler(CGI),
        reactor_(reactor),
        request_(request),
        config_(merge_config),
        output_buffer_(output_buffer),
        fd_(fd),
        pid_(-1),
        write_offset_(0)
{
    std::cout << "CgiHandler default constructor called" << std::endl;
    stdin_pipe_[0] = -1;
    stdin_pipe_[1] = -1;
    stdout_pipe_[0] = -1;
    stdout_pipe_[1] = -1;
    body_ = request_.getBody();
}

CgiHandler::~CgiHandler(void)
{
    std::cout << "CgiHandler destructor called" << std::endl;
    cleanup();
}

// --------------------------- Public Getter Methods ---------------------------

int CgiHandler::getFd(void) const
{
    return (fd_);
}

int CgiHandler::getReadFd() const
{
    return (stdout_pipe_[0]);
}

int CgiHandler::getWriteFd() const
{
    return (stdin_pipe_[1]);
}


// --------------------------- Public Member Methods ---------------------------

// collect child stdout (EPOLLIN on read-end)
void    CgiHandler::handleRead(void)
{
    std::cout << "CgiHandler handleRead called" << std::endl;
    char buffer[8192];
    while (true)
    {
        ssize_t bytes_read = read(stdout_pipe_[0], buffer, sizeof(buffer));
        if (bytes_read == 0) // EOF — child finished writing
        {
            std::cerr << "[CgiHandler::handleRead] EOF, output_buffer_ size="
                      << output_buffer_.size()
                      << " content='" << output_buffer_ << "'" << std::endl;
            reactor_.deleteHandler(stdout_pipe_[0]);
            close(stdout_pipe_[0]);
            stdout_pipe_[0] = -1;
            deactivate();
            reactor_.wakeUpHandler(fd_);
            return;
        }
        if (bytes_read == -1)
        {
            // TODO: remove usage of errno
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return; // more data coming, wait for next EPOLLIN
            handleError();
            return;
        }
        output_buffer_.append(buffer, bytes_read);
    }
}

// forward request body to child stdin (EPOLLOUT on write-end)
void    CgiHandler::handleWrite(void)
{
    std::cout << "CgiHandler handleWrite called" << std::endl;
    while (write_offset_ < body_.size())
    {
        ssize_t n = write(stdin_pipe_[1],
                         body_.data() + write_offset_,
                         body_.size() - write_offset_);
        if (n == -1)
        {
            // TODO: remove usage of errno
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return; // come back on next EPOLLOUT
            handleError();
            return;
        }
        write_offset_ += static_cast<size_t>(n);
    }
    // Done writing body → close child's stdin so it sees EOF
    reactor_.deleteHandler(stdin_pipe_[1]);
    close(stdin_pipe_[1]);
    stdin_pipe_[1] = -1;
    setWantWrite(false);
}

void    CgiHandler::handleError(void)
{
    std::cout << "CgiHandler handleError called" << std::endl;
    output_buffer_.clear();

    // Remove both pipe fds from epoll before deactivating
    if (stdin_pipe_[1] != -1)
    {
       reactor_.deleteHandler(stdin_pipe_[1]);
        close(stdin_pipe_[1]);
        stdin_pipe_[1] = -1;
    }
    if (stdout_pipe_[0] != -1)
    {
        reactor_.deleteHandler(stdout_pipe_[0]);
        close(stdout_pipe_[0]);
        stdout_pipe_[0] = -1;
    }

    deactivate();
    reactor_.wakeUpHandler(fd_);
}

bool CgiHandler::execCgi()
{
    if (pipe(stdin_pipe_) == -1)
        return (false);
    if (pipe(stdout_pipe_) == -1)
    {
        cleanup();
        return (false);
    }
    pid_ = fork();
    if ( pid_ == -1 )
    {
        cleanup();
        return (false);
    }
    if ( pid_ == 0)
    {
        close(stdin_pipe_[1]);
        dup2(stdin_pipe_[0], STDIN_FILENO);
        close(stdin_pipe_[0]);

        close(stdout_pipe_[0]);
        dup2(stdout_pipe_[1], STDOUT_FILENO);
        dup2(stdout_pipe_[1], STDERR_FILENO);
        close(stdout_pipe_[1]);

        const std::string                               cgi_uri = request_.getUri();
        size_t                                          dot_pos = cgi_uri.find_last_of('.');
        if (dot_pos == std::string::npos)
        {
            cleanup(); // TODO: really usefull here?
            std::exit(1);
        }
        const std::string                               cgi_extension = cgi_uri.substr(dot_pos);
        const std::map<std::string,std::string>&        cgi_map = config_.getCgi();
        if (cgi_map.empty())
        {
            cleanup(); // TODO: really usefull here?
            std::exit(1);
        }
        std::map<std::string, std::string>::const_iterator it = cgi_map.find(cgi_extension);
        if (it == cgi_map.end())
        {
            cleanup(); // TODO: really usefull here?
            std::exit(1);
        }
        const std::string   cgi_script_path = it->second;
        std::string         code_interpreter;

        if (cgi_extension == ".py")
            code_interpreter = "/usr/bin/python3";
        else if ( cgi_extension == ".php")
            code_interpreter = "/usr/bin/php-cgi";
        else
        {
            cleanup(); // TODO: really usefull here?
            std::exit(1);
        }
        if (access(cgi_script_path.c_str(), F_OK | X_OK) == -1)
        {
            cleanup();  // TODO: really usefull here?
            std::exit(1);
        }
        char *argv[] = {
            const_cast<char*>(code_interpreter.c_str()),
            const_cast<char*>(cgi_script_path.c_str()),
            NULL
        };

        // Build env variables
        std::vector<char*>       envp;
        buildEnvironmentVariables(envp);

        if (execve(argv[0], argv, envp.data()) == -1)
        {
            std::cerr << "Error executing CGI script!" << std::endl;
            cleanup();  // TODO: really usefull here? or better close STDIN_FILENO, STDOUT_FILENO and STDERR_FILENO directly?
            std::exit(1);
        }
    }

    close(stdin_pipe_[0]);
    stdin_pipe_[0]  = -1;
    close(stdout_pipe_[1]);
    stdout_pipe_[1] = -1;

    // make both parent-side fds non-blocking
    Utils::setNonBlocking(stdin_pipe_[1]);
    Utils::setNonBlocking(stdout_pipe_[0]);

    // Register the write-end only when there is a body to forward.
    // We use the explicit-fd overload because one CgiHandler covers two fds.
    if (!body_.empty())
        reactor_.addHandler(this, stdin_pipe_[1],  EPOLLOUT);
    else
    {
        // No body: close child stdin immediately so it sees EOF.
        close(stdin_pipe_[1]);
        stdin_pipe_[1] = -1;
    }

    // Always register the read-end to collect the child's output.
    setWantWrite(false);
    setWantRead(true);

    reactor_.addHandler(this, stdout_pipe_[0], EPOLLIN);

    return (true);
}


// --------------------------- Private Member Methods --------------------------

void    CgiHandler::cleanup()
{
    if (stdin_pipe_[0] != -1)
    {
        close(stdin_pipe_[0]);
        stdin_pipe_[0]  = -1;
    }
    if (stdin_pipe_[1] != -1)
    {
        close(stdin_pipe_[1]);
        stdin_pipe_[1]  = -1;
    }
    if (stdout_pipe_[0] != -1)
    {
        close(stdout_pipe_[0]);
        stdout_pipe_[0] = -1;
    }
    if (stdout_pipe_[1] != -1)
    {
        close(stdout_pipe_[1]);
        stdout_pipe_[1] = -1;
    }
    if (pid_ > 0) {
        kill(pid_, SIGKILL);
        waitpid(pid_, NULL, WNOHANG);
        pid_ = -1;
    }
}

void    CgiHandler::buildEnvironmentVariables( std::vector<char*>& envp )
{
    env_strings_.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env_strings_.push_back("SERVER_PROTOCOL=HTTP/1.1");
    env_strings_.push_back("SERVER_SOFTWARE=Webserv/1.0");
    env_strings_.push_back("REQUEST_METHOD=" + request_.getMethodToString());
    env_strings_.push_back("SCRIPT_FILENAME=" + request_.getUri());
    env_strings_.push_back("SCRIPT_NAME=" + request_.getUri());
    env_strings_.push_back("PATH_INFO=" + request_.getUri());
    env_strings_.push_back("CONTENT_LENGTH=" + Utils::toString(request_.getBody().size()));
    env_strings_.push_back("CONTENT_TYPE=" + request_.getHeaderValue(Http::Headers::CONTENT_TYPE));
    env_strings_.push_back("HTTP_HOST=" + request_.getHeaderValue(Http::Headers::HOST));
    env_strings_.push_back("HTTP_COOKIE=" + request_.getHeaderValue(Http::Headers::COOKIE)); // TODO: update with getCookies method

    for (size_t i = 0; i < env_strings_.size(); ++i)
        envp.push_back(const_cast<char*>(env_strings_[i].c_str()));
    envp.push_back(NULL);
}

