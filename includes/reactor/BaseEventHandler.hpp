/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseEventHandler.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgossard <jgossard@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:52:33 by jgossard          #+#    #+#             */
/*   Updated: 2026/03/11 18:07:07 by jgossard         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BASE_EVENT_HANDLER_HPP
#define BASE_EVENT_HANDLER_HPP

#include "IEventHandler.hpp"

class BaseEventHandler : public IEventHandler {
public:
    enum HandlerFlag {
        FLAG_NONE           = 0,
        FLAG_INACTIVE       = 1 << 0,
        FLAG_WANT_WRITE     = 1 << 1,
        FLAG_WANT_READ      = 1 << 2,
    };

    enum EventType {
        ACCEPTOR,
        CONNECTION,
    };

    // ---------- Constructors / Destructor ----------

    BaseEventHandler(EventType type);
    ~BaseEventHandler(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    EventType       getType() const;
    std::string     getTypeToString() const;
    void            setWantWrite(bool enable);
    void            setWantRead(bool enable);

    // ---------- Inherited Member Methods -----------------------

    virtual bool    isInactive() const;
    virtual void    deactivate();
    virtual bool    wantRead() const;
    virtual bool    wantWrite() const;

    // ---------- Public Member Methods -----------------------

    void    setFlag(HandlerFlag flag);
    void    clearFlag(HandlerFlag flag);
    bool    hasFlag(HandlerFlag flag) const;


protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    int         flags_;
    EventType   type_;

    BaseEventHandler(const BaseEventHandler& copy);
    BaseEventHandler& operator=(const BaseEventHandler& copy);
};

#endif // BASE_EVENT_HANDLER_HPP
