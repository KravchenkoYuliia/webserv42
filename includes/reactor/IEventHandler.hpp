/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IEventHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:09:30 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 16:25:45 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IEVENT_HANDLER_HPP
#define IEVENT_HANDLER_HPP

#include <string>

class IEventHandler {
public:
    virtual ~IEventHandler() {}

    // pure lifecycle interface
    // => Reactor only calls these
    // => Handlers will only have to implement these function
    virtual void    handleRead() = 0;
    virtual void    handleWrite() = 0;
    virtual void    handleError() = 0;
    virtual int     getFd() const = 0;

    // flags queries - implemented by BaseEventHandler
    virtual bool    isInactive() const = 0;
    virtual void    deactivate() = 0;
    virtual bool    wantRead() const = 0;
    virtual bool    wantWrite() const = 0;
};

#endif // IEVENT_HANDLER_HPP
