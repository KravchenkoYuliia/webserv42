/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:19:32 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 18:10:36 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <string>
#include <vector>
#include <inttypes.h> // uint32_t
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

    void        addHandler( IEventHandler *handler );
    void        updateHandler( IEventHandler *handler );
    void        deleteHandler( int fd );
    void        removeDeactivatedHandler();
    void        enableWrite(IEventHandler *handler);
    void        disableWrite(IEventHandler *handler);
    void        run(); // TODO: should it be renamed to handleEvent?
    uint32_t    computeEvents(IEventHandler *handler);

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int                           epoll_fd_;
    std::vector<IEventHandler *>  handlers_;

    Reactor(const Reactor& copy);
    Reactor& operator=(const Reactor& copy);

};

#endif // REACTOR_HPP
