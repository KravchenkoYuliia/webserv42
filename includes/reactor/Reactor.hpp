/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:19:32 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/02 11:03:42 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <string>
#include <vector>
#include "reactor/IEventHandler.hpp"

/*
    Ex-EventLoop class

    It should ONLY:
        - registerHandler(fd, handler, events)
        - removeHandler(fd)
        - eventLoop()
    It must NOT:
        - parse HTTP
        - manage connections
        - build responses


Reactor triggers events.
Connection changes its own state.

Reactor does not manage HTTP state.
*/

class Reactor {
public:
    // ---------- Constructors / Destructor ----------

    Reactor(void);
    ~Reactor(void);

    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------

    void    addHandler( IEventHandler *handler, uint32_t epoll_event_type);
    void    updateHandler( IEventHandler *handler, uint32_t epoll_event_type );
    void    deleteHandler( int fd );
    void    run(); // TODO: should it be renamed to handleEvent?

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int                             epoll_fd_;
    std::vector<IEventHandler *>  handlers_;

    Reactor(const Reactor& copy);
    Reactor& operator=(const Reactor& copy);

};

#endif // REACTOR_HPP
