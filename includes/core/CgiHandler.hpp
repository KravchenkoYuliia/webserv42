/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 10:59:41 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 16:11:10 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HANDLER_HPP
#define CGI_HANDLER_HPP

#include <string>
#include <vector>
#include "core/ConnectionHandler.hpp"
#include "http/HttpRequest.hpp"
#include "reactor/BaseEventHandler.hpp"
#include "reactor/Reactor.hpp"
#include "routing/MergedConfig.hpp"

class CgiHandler: public BaseEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    CgiHandler(
        Reactor& reactor,
        const HttpRequest& request,
        const MergedConfig& merge_config,
        std::string& output_buffer,
        int fd
    );

    ~CgiHandler();

    // ---------- Overloading Operators Methods -------
    // ---------- Getter and Setter Methods ------------

    int             getReadFd() const;
    int             getWriteFd() const;

    // ---------- Member Methods -----------------------

    virtual void    handleRead(void);
    virtual void    handleWrite(void);
    virtual void    handleError(void);
    virtual int     getFd(void) const;

    bool            execCgi();

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------

    Reactor&            reactor_;
    const HttpRequest&  request_;
    const MergedConfig& config_;
    std::string&        output_buffer_;
    int                 fd_;
    pid_t               pid_;
    int                 stdin_pipe_[2];   // used to WRITE the request to CGI's stdin         => [0] = read, [1] = write
    int                 stdout_pipe_[2];  // used to READ the response from the CGI's stdout  => [0] = read, [1] = write
    std::string         body_;
    size_t              write_offset_;
    std::vector<std::string> env_strings_;

    // ---------- Private Helper Method -------------------------

    void    cleanup();
    void    buildEnvironmentVariables( std::vector<char*>& envp, const std::string cgi_path);

    // non-copyable
    CgiHandler(const CgiHandler& copy);
    CgiHandler& operator=(const CgiHandler& copy);

};

#endif // CGI_HANDLER_HPP
