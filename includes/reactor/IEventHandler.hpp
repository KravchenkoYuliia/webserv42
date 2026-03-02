/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IEventHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/27 11:09:30 by jgossard          #+#    #+#             */
/*   Updated: 2026/02/27 17:58:09 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IEVENT_HANDLER_HPP
#define IEVENT_HANDLER_HPP

#include <string>

class IEventHandler {
public:
    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;
    virtual void handleError() = 0;
    // virtual void handleEvent() = 0;
    virtual int getFd() const = 0;
    virtual ~IEventHandler() {}
};

#endif // IEVENT_HANDLER_HPP
