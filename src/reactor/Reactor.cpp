/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:54:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 11:13:28 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/epoll.h>      // epoll_create
#include <unistd.h>         // close
#include <stdexcept>
#include <errno.h>
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
    // delete handlers_ because Reactor owns handlers
    for (std::vector<IEventHandler*>::iterator it = handlers_.begin(); it < handlers_.end(); ++it)
        delete *it;
    handlers_.clear();
    if (epoll_fd_ != -1)
        close(epoll_fd_);
}

void Reactor::addHandler( IEventHandler *handler, uint32_t epoll_event_type )
{
    if (handler == NULL)
        throw std::invalid_argument("handler is NULL");

    // if push_back failed and throw a bad_alloc
    //-> handler will be freed by the catch statement in ServerManager::init()
    handlers_.push_back(handler);

    int fd = handler->getFd();

    struct epoll_event event;
    event.events = epoll_event_type;
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        // remove handler from handlers_ if epoll_ctl fails
        //-> handler will be freed by the catch statement in ServerManager::init() since Reactor does not own the handler
        handlers_.pop_back();
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Reactor::updateHandler( IEventHandler *handler, uint32_t epoll_event_type)
{
    int fd = handler->getFd();

    struct epoll_event event;
    event.events = epoll_event_type;
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
            delete *it;
            handlers_.erase(it);
            break;
        }
    }
}

void Reactor::run()
{
    const int kMaxReadyEventsBatchSize = 1024;
    struct epoll_event ready_events_list[kMaxReadyEventsBatchSize];

    while (true)
    {
        int num_fds_ready = epoll_wait(epoll_fd_, ready_events_list, kMaxReadyEventsBatchSize, -1);
        if (num_fds_ready == -1)
        {
            if (errno == EINTR) // TODO: check in which case we can hit this check
                continue;
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < num_fds_ready; ++i)
        {
            IEventHandler *handler = static_cast<IEventHandler *>(ready_events_list[i].data.ptr);
            if (ready_events_list[i].events & (EPOLLERR | EPOLLHUP))
            {
                handler->handleError();
                continue; // skip read / write if error
                // TODO: mark the events as "deactivate" and add a safe deletion after the for loop
            }
            if (ready_events_list[i].events & EPOLLIN) //  TODO: should add EPOLLET here?
                handler->handleRead();
            if (ready_events_list[i].events & EPOLLOUT) // TODO: should add EPOLLET here?
                handler->handleWrite();
        }
        // TODO: safe delete  all the deactivate handler
    }
}
