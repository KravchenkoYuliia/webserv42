/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseEventHandler.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 10:43:36 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/31 13:00:17 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "reactor/BaseEventHandler.hpp"

// ############################# HttpRequest Class #############################

// ------------------------- Destructor / Constructor -------------------------

BaseEventHandler::BaseEventHandler(EventType type)
    :   flags_(FLAG_WANT_READ),
        type_(type)
{
}

BaseEventHandler::~BaseEventHandler(void)
{
    // TODO: remove this log

    std::cout << "BaseEventHandler destructor called" << std::endl;
}

// --------------------------- Public Getter Methods ---------------------------

BaseEventHandler::EventType   BaseEventHandler::getType() const
{
    return (type_);
}

std::string   BaseEventHandler::getTypeToString() const
{
    if (type_ == CONNECTION)
        return ("CONNECTION");
    else if (type_ == CGI)
        return ("CGI");
    return ("ACCEPTOR");
}

void    BaseEventHandler::setWantWrite(bool enable)
{
    if (enable)
        setFlag(FLAG_WANT_WRITE);
    else
        clearFlag(FLAG_WANT_WRITE);
}

void    BaseEventHandler::setWantRead(bool enable)
{
    if (enable)
        setFlag(FLAG_WANT_READ);
    else
        clearFlag(FLAG_WANT_READ);
}

// --------------------------- Public Inherited Member Methods ---------------------------

bool    BaseEventHandler::isInactive() const
{
   return (hasFlag(FLAG_INACTIVE));
}

void    BaseEventHandler::deactivate()
{
    setFlag(FLAG_INACTIVE);
}

bool    BaseEventHandler::wantRead() const
{
    return (hasFlag(FLAG_WANT_READ));
}

bool    BaseEventHandler::wantWrite() const
{
    return (hasFlag(FLAG_WANT_WRITE));
}

// --------------------------- Public Member Methods ---------------------------

void    BaseEventHandler::setFlag(HandlerFlag flag)
{
    flags_ |= flag;
}

void    BaseEventHandler::clearFlag(HandlerFlag flag)
{
    flags_ &= ~flag;
}

bool    BaseEventHandler::hasFlag(HandlerFlag flag) const
{
    return (( flags_ & flag ) != 0);
}

