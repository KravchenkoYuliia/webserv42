# Question

let's go back to the flags, you have suggested to have DEAD, WRITABLE and created boolean variable accordingly
can it be done differently? because i am not a big fan of using bool variable, can i use somethings like int flag and create an enum with all the possible flag?
can you suggest others way to do it instead of int?

then for both can you tell me what to change regarding the earlier flags solution?

Great question. Here are the main alternatives:

---

## Option 1: Bitmask enum with `int`

```cpp
enum HandlerFlag {
    FLAG_NONE     = 0,
    FLAG_DEAD     = 1 << 0,  // 0001
    FLAG_WRITABLE = 1 << 1,  // 0010
    FLAG_CGI_DONE = 1 << 2,  // 0100
    // add future flags here cheaply
};
```

```cpp
// in IEventHandler — one single int replaces all bools
int flags_;

// manipulate
void   setFlag(HandlerFlag f)   { flags_ |= f; }
void   clearFlag(HandlerFlag f) { flags_ &= ~f; }
bool   hasFlag(HandlerFlag f) const { return (flags_ & f) != 0; }

// exact same API as before, just wraps the bitmask
bool isDead()      const { return hasFlag(FLAG_DEAD); }
void markDead()          { setFlag(FLAG_DEAD); }
bool isWritable()  const { return hasFlag(FLAG_WRITABLE); }
void setWritable(bool v) { v ? setFlag(FLAG_WRITABLE) : clearFlag(FLAG_WRITABLE); }
```

**Pros:** one `int` instead of N bools, trivially extensible, idiomatic in C/C++ systems code, easy to print/debug all flags at once.
**Cons:** slightly less readable at the bit-manipulation layer (hidden behind helpers though).

---

## Option 2: State machine enum

Instead of independent flags, model the connection as explicit states:

```cpp
enum ConnectionState {
    STATE_READING,        // waiting for full request
    STATE_WRITING,        // sending response
    STATE_CLOSING,        // response sent, pending close
    STATE_DEAD            // marked for deletion
};
```

```cpp
ConnectionState state_;

// in IEventHandler
bool isDead()     const { return state_ == STATE_DEAD; }
void markDead()         { state_ = STATE_DEAD; }
bool isWritable() const { return state_ == STATE_WRITING; }
```

**Pros:** very readable, forces you to think about valid transitions, impossible to be in contradictory states (e.g. DEAD + WRITING at once).
**Cons:** flags that are truly independent (like `FLAG_WRITABLE` and `FLAG_CGI_DONE`) don't map cleanly to a linear state — you'd need combined states like `STATE_WRITING_CGI_DONE` which explodes quickly.

---

## Recommendation for webserv

Use **both together** — bitmask for independent flags, state machine for the connection lifecycle:

```cpp
// IEventHandler.hpp

enum HandlerFlag {
    FLAG_NONE     = 0,
    FLAG_DEAD     = 1 << 0,
    FLAG_WRITABLE = 1 << 1,
    FLAG_CGI_DONE = 1 << 2
};

class IEventHandler
{
public:
    enum Type { ACCEPTOR, CONNECTION, CGI };

    IEventHandler(Type type)
        : flags_(FLAG_NONE)
        , type_(type)
    {}

    virtual ~IEventHandler() {}
    virtual void handleRead()  = 0;
    virtual void handleWrite() = 0;
    virtual void handleError() = 0;
    virtual int  getFd() const = 0;

    // generic bitmask API
    void setFlag(HandlerFlag f)      { flags_ |= f; }
    void clearFlag(HandlerFlag f)    { flags_ &= ~f; }
    bool hasFlag(HandlerFlag f) const { return (flags_ & f) != 0; }

    // named helpers — same interface as before, nothing changes in Reactor/Handlers
    bool isDead()      const { return hasFlag(FLAG_DEAD); }
    void markDead()          { setFlag(FLAG_DEAD); }

    bool isWritable()  const { return hasFlag(FLAG_WRITABLE); }
    void setWritable(bool v) { v ? setFlag(FLAG_WRITABLE) : clearFlag(FLAG_WRITABLE); }

    Type getType()     const { return type_; }

private:
    int  flags_;   // replaces all individual bools
    Type type_;
};
```

**Nothing else changes** — `Reactor.cpp`, `ConnectionHandler.cpp`, `ConnectionAcceptor.cpp` all call the same `isDead()`, `markDead()`, `isWritable()`, `setWritable()` as before. The bitmask is purely an internal implementation detail behind the named helpers.

---

## Adding a future flag costs one line

```cpp
// today
FLAG_CGI_DONE = 1 << 2,

// tomorrow — zero refactor
FLAG_KEEP_ALIVE = 1 << 3,
FLAG_TLS        = 1 << 4,
```

And you can debug all flags at once easily:

```cpp
void IEventHandler::printFlags() const
{
    std::cout << "fd=" << getFd()
              << " DEAD="     << hasFlag(FLAG_DEAD)
              << " WRITABLE=" << hasFlag(FLAG_WRITABLE)
              << " CGI_DONE=" << hasFlag(FLAG_CGI_DONE)
              << std::endl;
}
```

