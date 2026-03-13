/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:54:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/13 16:09:10 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/epoll.h>      // epoll_create
#include <unistd.h>         // close
#include <stdexcept>
#include <errno.h>
#include "core/SignalHandler.hpp"
#include "reactor/Reactor.hpp"

Reactor::Reactor(void)
{
    std::cout << "Reactor default constructor called" << std::endl;

    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1)
        throw std::runtime_error("epoll_create failed");
}

Reactor::~Reactor(void)
{
    for (std::vector<IEventHandler*>::iterator it = handlers_.begin(); it < handlers_.end(); ++it)
        delete *it;
    handlers_.clear();
    if (epoll_fd_ != -1)
        close(epoll_fd_);
}

void Reactor::addHandler( IEventHandler *handler )
{
    if (handler == NULL)
        throw std::invalid_argument("handler is NULL");

    // TODO: remove these logs
    // if push_back failed and throw a bad_alloc
    //-> handler will be freed by the catch statement in ServerManager::init()
    handlers_.push_back(handler);

    int fd = handler->getFd();

    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        // TODO: remove these logs
        // remove handler from handlers_ if epoll_ctl fails
        //-> handler will be freed by the catch statement in ServerManager::init() since Reactor does not own the handler
        handlers_.pop_back();
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Reactor::updateHandler( IEventHandler *handler )
{
    int fd = handler->getFd();

    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_MOD, fd, &event) == -1)
        throw std::runtime_error("epoll_ctl MOD failed");
}

void Reactor::deleteHandler( int fd )
{
    if (epoll_ctl( epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("epoll_ctl DEL failed");
    for (std::vector<IEventHandler* >::iterator it = handlers_.begin(); it != handlers_.end(); ++it)
    {
        if ((*it)->getFd() == fd)
        {
            (*it)->deactivate();
            break;
        }
    }
}

void Reactor::removeDeactivatedHandler()
{
    for (std::vector<IEventHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); )
    {
        if ((*it)->isInactive())
        {
            delete *it;
            // erase reallocate the vector leading to iterator invalidation
            // => need to reallocate iterator by the returning iterator pointer
            it = handlers_.erase(it);
        }
        else
            ++it;
    }
}

uint32_t    Reactor::computeEvents(IEventHandler *handler)
{
    uint32_t events = EPOLLET;

    if (handler->wantRead())
        events |= EPOLLIN;

    if (handler->wantWrite())
        events |= EPOLLOUT;
    return (events);
}

void Reactor::run()
{
    if (!g_running)
        return ;

    const int kMaxReadyEventsBatchSize = 1024;
    struct epoll_event ready_events_list[kMaxReadyEventsBatchSize];

    while (g_running)
    {
        int num_fds_ready = epoll_wait(epoll_fd_, ready_events_list, kMaxReadyEventsBatchSize, -1);
        if (num_fds_ready == -1)
        {
            if (errno == EINTR) // TODO: check in which case we can hit this check/ can we keep it?
                continue;
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < num_fds_ready; ++i)
        {
            const epoll_event &ev = ready_events_list[i];
            IEventHandler *handler = static_cast<IEventHandler *>(ev.data.ptr);
            if (handler->isInactive())
                continue;
            if (ev.events & (EPOLLERR | EPOLLHUP))
            {
                handler->handleError();
                continue; // skip read / write if error
            }
            if (ev.events & EPOLLIN)
                handler->handleRead();
            if ((ev.events & EPOLLOUT) && !handler->isInactive())
                handler->handleWrite();
        }
        // safe deletion of all the inactived handler
        removeDeactivatedHandler();
    }
}
