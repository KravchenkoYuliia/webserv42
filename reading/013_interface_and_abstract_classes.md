# question

by adding priavte variable and constructor in the IEventHandler, i will transform this class from an interface to an abstract class type...is it not contrary to the IEventHandler class utility as stated in the reactor design pattern that i am trying to follow?
how could i keep my class as an interface and still implement the flags and state machine?

# answer

Excellent observation — you're absolutely right. A true interface should have zero data and zero implementation.

---

## The problem

```cpp
// this is no longer an interface — it has state and implementation
class IEventHandler {
    int  flags_;   // data → abstract class
    Type type_;    // data → abstract class
    void setFlag(HandlerFlag f) { flags_ |= f; } // implementation → abstract class
};
```

---

## The solution: split into two classes

Keep `IEventHandler` as a **pure interface**, add `BaseEventHandler` as an **abstract base class** that implements the common plumbing:

```
IEventHandler          ← pure interface, zero data, only virtual methods
      ↑
BaseEventHandler       ← owns flags_, type_, all flag logic
      ↑              ↑
ConnectionHandler   ConnectionAcceptor   ← own state_, business logic
```

---

## IEventHandler.hpp — restored to pure interface

```cpp
#ifndef IEVENTHANDLER_HPP
#define IEVENTHANDLER_HPP

#include <stdint.h>

class IEventHandler
{
public:
    virtual ~IEventHandler() {}

    // pure lifecycle interface — Reactor only calls these
    virtual void handleRead()  = 0;
    virtual void handleWrite() = 0;
    virtual void handleError() = 0;
    virtual int  getFd()       const = 0;

    // flag queries — pure, implemented by BaseEventHandler
    virtual bool isDead()      const = 0;
    virtual void markDead()          = 0;
    virtual bool isWritable()  const = 0;
    virtual void setWritable(bool v) = 0;
};

#endif
```

---

## BaseEventHandler.hpp — owns all flag logic

```cpp
#ifndef BASEEVENTHANDLER_HPP
#define BASEEVENTHANDLER_HPP

#include "IEventHandler.hpp"

enum HandlerFlag {
    FLAG_NONE     = 0,
    FLAG_DEAD     = 1 << 0,
    FLAG_WRITABLE = 1 << 1,
    FLAG_CGI_DONE = 1 << 2
};

class BaseEventHandler : public IEventHandler
{
public:
    enum Type { ACCEPTOR, CONNECTION, CGI };

    explicit BaseEventHandler(Type type)
        : flags_(FLAG_NONE)
        , type_(type)
    {}

    virtual ~BaseEventHandler() {}

    // IEventHandler flag interface — implemented here once for all subclasses
    virtual bool isDead()      const { return hasFlag(FLAG_DEAD); }
    virtual void markDead()          { setFlag(FLAG_DEAD); }
    virtual bool isWritable()  const { return hasFlag(FLAG_WRITABLE); }
    virtual void setWritable(bool v) { v ? setFlag(FLAG_WRITABLE)
                                        : clearFlag(FLAG_WRITABLE); }

    // generic bitmask API — available to all subclasses
    void setFlag(HandlerFlag f)       { flags_ |=  f; }
    void clearFlag(HandlerFlag f)     { flags_ &= ~f; }
    bool hasFlag(HandlerFlag f) const { return (flags_ & f) != 0; }

    Type getType() const { return type_; }

private:
    int  flags_;
    Type type_;
};

#endif
```

---

## What changes in the concrete handlers

Just the base class they inherit from — nothing else:

```cpp
// before
class ConnectionHandler : public IEventHandler

// after
class ConnectionHandler : public BaseEventHandler
{
public:
    ConnectionHandler(int fd, Reactor &reactor, const std::string &root_dir)
        : BaseEventHandler(BaseEventHandler::CONNECTION)  // ← only change
        , fd_(fd)
        , reactor_(reactor)
        , builder_(root_dir)
        , write_offset_(0)
        , state_(STATE_READING)
    {}
    // ... everything else identical
};

// same for ConnectionAcceptor
class ConnectionAcceptor : public BaseEventHandler
{
public:
    ConnectionAcceptor(Socket *socket, Reactor &reactor)
        : BaseEventHandler(BaseEventHandler::ACCEPTOR)    // ← only change
        , socket_(socket)
        , reactor_(reactor)
    {}
};
```

---

## Reactor doesn't change at all

```cpp
// Reactor still only knows IEventHandler* — untouched
void Reactor::addHandler(IEventHandler *handler, uint32_t events);
void Reactor::deleteHandler(int fd);
void Reactor::enableWrite(IEventHandler *handler);
// etc.
```

---

## The class hierarchy is now correct

| Class | Type | Has data? | Has implementation? |
|---|---|---|---|
| `IEventHandler` | Pure interface | no | no |
| `BaseEventHandler` | Abstract base | yes (`flags_`, `type_`) | yes (flag logic) |
| `ConnectionHandler` | Concrete | yes (`fd_`, `state_`…) | yes (HTTP logic) |
| `ConnectionAcceptor` | Concrete | yes (`socket_`…) | yes (accept logic) |

The Reactor pattern is preserved — Reactor only ever holds and calls `IEventHandler*`, remaining completely decoupled from all implementation details.
