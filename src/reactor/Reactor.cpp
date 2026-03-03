/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:54:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/03 19:00:02 by jgossard         ###   ########.fr       */
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
    // TODO: not sure if Reactor really own handlers_ or not, to clarify after implementation of ServerManager

    // delete handlers_ if Reactor owns handlers
    for (std::vector<IEventHandler*>::iterator it = handlers_.begin(); it < handlers_.end(); ++it)
        delete *it;
    handlers_.clear();
    // close fd // TODO: remove this comment
    if (epoll_fd_ != -1)
        close(epoll_fd_);
}

void Reactor::addHandler( IEventHandler *handler, uint32_t epoll_event_type )
{
    if (handler == NULL)
        throw std::invalid_argument("handler is NULL");

    int fd = handler->getFd();

    struct epoll_event event;
    event.events = epoll_event_type;
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
        throw std::runtime_error("epoll_ctl ADD failed");
    handlers_.push_back(handler);
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
    int kMaxReadyEventsBatchSize = 1024;
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
            if (ready_events_list[i].events & EPOLLIN)
                handler->handleRead();
            if (ready_events_list[i].events & EPOLLOUT)
                handler->handleWrite();
            if (ready_events_list[i].events & (EPOLLERR | EPOLLHUP))
                handler->handleError();
        }
    }
}
