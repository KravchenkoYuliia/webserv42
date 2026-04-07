/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:19:32 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 16:11:27 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <string>
#include <vector>
#include <map>
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
    void        addHandler( IEventHandler* handler, int fd );
    void        addHandler(IEventHandler* handler, int fd, uint32_t events);

    void        updateHandler( IEventHandler *handler );
    void        deleteHandler( int fd );
    void        removeDeactivatedHandler();
    void        run();
    uint32_t    computeEvents(IEventHandler *handler);
    void        wakeUpHandler(int fd);


protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int                           epoll_fd_;
    std::vector<IEventHandler *>  handlers_;
    std::map<int, IEventHandler *>  fd_map_;


    bool        isHandlerRegistered( IEventHandler* handler ) const;

    Reactor(const Reactor& copy);
    Reactor& operator=(const Reactor& copy);

};

#endif // REACTOR_HPP
