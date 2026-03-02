#ifndef ACCEPTOR_HPP
#define ACCEPTOR_HPP

#include "core/Socket.hpp"
#include "reactor/IEventHandler.hpp"
#include "reactor/Reactor.hpp"

/*
    Acceptor (Listening Socket Handler)
    This handles:
        - Listens on a server socket
        - EPOLLIN on server socket
        - Accept new clients
        - Create new Connection objects
        - Register them into Reactor
*/

class Acceptor : public IEventHandler {
public:
    // ---------- Constructors / Destructor ----------

    Acceptor(Socket *socket, Reactor& reactor);
    ~Acceptor(void);

    // ---------- Overloading Operators Methods -------


    // ---------- Getter and Setter Methods ------------

    // ---------- Member Methods -----------------------
    virtual void handleRead();
    virtual void handleWrite();
    virtual void handleError();
    // virtual void handleEvent();
    virtual int  getFd() const;

protected:
    // ---------- Protected Data Members ---------------------

private:
    // ---------- Private Data members -------------------------
    Socket      *server_socket_;
    Reactor&    reactor_;

    Acceptor(const Acceptor& copy);
    Acceptor& operator=(const Acceptor& copy);

};

#endif // ACCEPTOR_HPP
