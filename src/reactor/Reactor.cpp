/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reactor.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:54:17 by jgossard          #+#    #+#             */
/*   Updated: 2026/04/07 18:25:38 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sys/epoll.h>      // epoll_create
#include <unistd.h>         // close
#include <stdexcept>
#include <errno.h>          // errno, EINTR
#include <cstring>          //strerror
#include <algorithm>        // std::find
#include "core/SignalHandler.hpp"
#include "reactor/Reactor.hpp"
#include "reactor/BaseEventHandler.hpp"

Reactor::Reactor(void)
{
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

    // if push_back failed and throw a bad_alloc
    //-> handler will be freed by the catch statement in ServerManager::init()
    handlers_.push_back(handler);
    fd_map_[handler->getFd()] = handler;

    int fd = handler->getFd();

    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        // remove handler from handlers_ if epoll_ctl fails
        //-> handler will be freed by the catch statement in ServerManager::init() since Reactor does not own the handler
        handlers_.pop_back();
        fd_map_.erase(handler->getFd());
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

bool Reactor::isHandlerRegistered(IEventHandler* handler) const
{
    for (std::vector<IEventHandler *>::const_iterator it = handlers_.begin();
        it != handlers_.end(); ++it)
    {
        if (*it == handler)
            return (true);
    }
    return (false);
}

void Reactor::addHandler(IEventHandler* handler, int fd)
{
    if (handler == NULL)
        throw std::invalid_argument("handler is NULL");

    if (!isHandlerRegistered(handler))
        handlers_.push_back(handler);
    fd_map_[fd] = handler;

    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;

    if (epoll_ctl( epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        fd_map_.erase(fd);
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Reactor::addHandler(IEventHandler* handler, int fd, uint32_t events)
{
    if (handler == NULL)
        throw std::invalid_argument("handler is NULL");

    if (!isHandlerRegistered(handler))
        handlers_.push_back(handler);
    fd_map_[fd] = handler;
    struct epoll_event event;
    event.events   = events | EPOLLET;
    event.data.ptr = handler;

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        fd_map_.erase(fd);
        throw std::runtime_error("epoll_ctl ADD failed");
    }
}

void Reactor::updateHandler( IEventHandler *handler )
{
    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;
    for (std::map<int, IEventHandler* >::iterator it = fd_map_.begin(); it != fd_map_.end(); ++it)
    {
        if (it->second == handler)
        {
            if (epoll_ctl( epoll_fd_, EPOLL_CTL_MOD, it->first, &event) == -1)
            {
                std::cerr << "[Reactor::updateHandler] epoll_ctl MOD failded for fd : " << it->first << std::endl;
                throw std::runtime_error("[Reactor::updateHandler] epoll_ctl MOD failed");
            }
        }
    }
}

void Reactor::deleteHandler( int fd )
{
    if (epoll_ctl( epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("epoll_ctl DEL failed");
    std::map<int, IEventHandler* >::iterator map_it = fd_map_.find(fd);
    if (map_it == fd_map_.end())
    {
        std::cerr << "[Reactor::deleteHandler] in case map_it == fd_map_.end()" << std::endl;
        return ;
    }
    IEventHandler *handler = map_it->second;
    fd_map_.erase(map_it);

    bool has_others_fds = false;
    for (std::map<int, IEventHandler* >::iterator it = fd_map_.begin(); it != fd_map_.end(); ++it)
    {
        if (it->second == handler)
        {
            has_others_fds = true;
            break;
        }
    }
    if (!has_others_fds)
        handler->deactivate();
}

void Reactor::removeDeactivatedHandler()
{
    for (std::vector<IEventHandler *>::iterator it = handlers_.begin(); it != handlers_.end(); )
    {
        if ((*it)->isInactive())
        {
            IEventHandler *handler = *it;
            for (std::map<int, IEventHandler *>::iterator map_it = fd_map_.begin(); map_it != fd_map_.end(); )
            {
                if (map_it->second == handler)
                {
                    std::map<int, IEventHandler *>::iterator tmp = map_it;
                    ++map_it;
                    fd_map_.erase(tmp);
                }
                else
                    ++map_it;
            }
            delete handler;
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
            if (errno == EINTR)
            {
                std::cerr << "\n[Reactor::run] CTRL-C intercepted" << std::endl;
                continue;
            }
            throw std::runtime_error("epoll_wait failed");
        }
        for (int i = 0; i < num_fds_ready; ++i)
        {
            const epoll_event &ev = ready_events_list[i];
            IEventHandler *handler = static_cast<IEventHandler *>(ev.data.ptr);
            if (handler->isInactive())
                continue;
            if (ev.events & (EPOLLERR))
            {
                handler->handleError();
                continue; // skip read / write if error
            }
            if (ev.events & EPOLLHUP)
            {
                BaseEventHandler *base = static_cast<BaseEventHandler *>(handler);
                if (base->getType() == BaseEventHandler::CGI)
                    handler->handleRead();
                else
                    handler->handleError();
                continue;
            }
            if (ev.events & EPOLLIN || ev.events & EPOLLHUP)
                handler->handleRead();
            if ((ev.events & EPOLLOUT) && !handler->isInactive())
                handler->handleWrite();
        }
        // safe deletion of all the inactived handler
        removeDeactivatedHandler();
    }
}

void Reactor::wakeUpHandler(int fd)
{
    std::map<int, IEventHandler *>::iterator it = fd_map_.find(fd);
    if (it == fd_map_.end())
        return ;
    IEventHandler *handler = it->second;
    if (handler->isInactive())
        return;
    handler->setWantWrite(true);
    handler->setWantRead(false);
    struct epoll_event event;
    event.events = computeEvents(handler);
    event.data.ptr = handler;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event) == -1)
        throw std::runtime_error("epoll_ctl MOD failed in wakeUpHandler");
}
