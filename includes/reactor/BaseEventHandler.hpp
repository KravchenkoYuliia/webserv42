/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseEventHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:52:33 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/04 12:21:55 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE_EVENT_HANDLER_HPP
#define BASE_EVENT_HANDLER_HPP

#include "IEventHandler.hpp"

class BaseEventHandler : public IEventHandler {
public:

    enum EventType {
        ACCEPTOR,
        CONNECTION,
    };

    // ---------- Constructors / Destructor ----------

    BaseEventHandler(void);
    BaseEventHandler(const BaseEventHandler& copy);
    ~BaseEventHandler(void);

    // ---------- Overloading Operators Methods -------

    BaseEventHandler& operator=(const BaseEventHandler& copy);

    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------


protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int         flags_;
    EventType   type_;
};

#endif // BASE_EVENT_HANDLER_HPP
