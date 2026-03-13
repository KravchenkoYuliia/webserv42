
## Questions

you suggested to "Use both together — bitmask for independent flags, state machine for the connection lifecycle:" but in the example code you only show an enum that only use bitmask...i don't get how you use the state machine for the connection lifecycle in your example

## Answer

You're right, I didn't show the state machine part at all. Let me fix that properly.

The idea is that these two things track **different concerns**:

- **State machine** → "what phase is this connection in?" (lifecycle, sequential, mutually exclusive)
- **Bitmask flags** → "what properties does this handler have right now?" (independent, combinable)

---

## ConnectionState lives in `ConnectionHandler` — not `IEventHandler`

The state machine belongs in `ConnectionHandler` specifically because `ConnectionAcceptor` has no lifecycle phases — it just always reads. Putting it in the base class would be meaningless for acceptors.

```cpp
// ConnectionHandler.hpp

class ConnectionHandler : public IEventHandler
{
public:
    enum ConnectionState {
        STATE_READING,   // accumulating request bytes
        STATE_WRITING,   // sending response
        STATE_CLOSING    // response fully sent, close after drain
        // note: no STATE_DEAD here — that's FLAG_DEAD in the bitmask
    };
    // ...
private:
    ConnectionState  state_;  // lifecycle — mutually exclusive phases
    // flags_ (FLAG_DEAD, FLAG_WRITABLE etc) inherited from IEventHandler
};
```

---

## How they work together

```
FLAG_DEAD    → owned by IEventHandler (Reactor needs it for ALL handler types)
FLAG_WRITABLE → owned by IEventHandler (Reactor::enableWrite/disableWrite use it)
FLAG_CGI_DONE → owned by IEventHandler (Reactor/CGIHandler interaction)

ConnectionState → owned by ConnectionHandler only (internal lifecycle logic)
```

---

## ConnectionHandler.cpp — showing both in use

```cpp
// ── READ ──────────────────────────────────────────────────────────────────────
void ConnectionHandler::handleRead()
{
    // state guard — ignore stale EPOLLIN if we already moved on
    if (state_ != STATE_READING)
        return;

    char buf[4096];
    while (true)
    {
        ssize_t n = recv(fd_, buf, sizeof(buf), 0);
        if (n == 0)  { reactor_.deleteHandler(fd_); return; }
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            reactor_.deleteHandler(fd_); return;
        }
        if (!parser_.feed(buf, static_cast<size_t>(n)))
        {
            state_ = STATE_CLOSING;       // state machine: parse error → close after error response
            HttpResponse err = buildError(400);
            write_buffer_ = err.serialize();
            write_offset_ = 0;
            reactor_.enableWrite(this);   // bitmask: arm EPOLLOUT
            return;
        }
        if (parser_.isComplete())
        {
            processRequest();
            return;
        }
    }
}

void ConnectionHandler::processRequest()
{
    HttpResponse response = builder_.build(parser_.getRequest());
    write_buffer_ = response.serialize();
    write_offset_ = 0;

    state_ = STATE_WRITING;           // state machine: advance lifecycle
    reactor_.enableWrite(this);       // bitmask: arm EPOLLOUT
}

// ── WRITE ─────────────────────────────────────────────────────────────────────
void ConnectionHandler::handleWrite()
{
    // state guard — should never fire outside these two states
    if (state_ != STATE_WRITING && state_ != STATE_CLOSING)
        return;

    while (write_offset_ < write_buffer_.size())
    {
        ssize_t n = send(fd_,
                         write_buffer_.data() + write_offset_,
                         write_buffer_.size() - write_offset_, 0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) return;
            reactor_.deleteHandler(fd_); return;
        }
        write_offset_ += static_cast<size_t>(n);
    }

    // buffer fully drained
    reactor_.disableWrite(this);      // bitmask: disarm EPOLLOUT

    if (state_ == STATE_CLOSING || shouldClose())
    {
        reactor_.deleteHandler(fd_);  // bitmask: sets FLAG_DEAD
        return;
    }

    // keep-alive: reset for next request
    state_ = STATE_READING;           // state machine: back to start
    parser_.reset();
}

// ── ERROR ─────────────────────────────────────────────────────────────────────
void ConnectionHandler::handleError()
{
    // state doesn't matter — hard error always kills the connection
    reactor_.deleteHandler(fd_);      // bitmask: sets FLAG_DEAD
}
```

---

## The clear separation

| Question | Answered by | Example |
|---|---|---|
| "Should the reactor delete this handler?" | `FLAG_DEAD` | `reapDeadHandlers()` |
| "Is EPOLLOUT currently registered?" | `FLAG_WRITABLE` | `enableWrite/disableWrite` |
| "What should `handleRead` do right now?" | `ConnectionState` | ignore if `STATE_WRITING` |
| "What should `handleWrite` do right now?" | `ConnectionState` | close vs keep-alive after drain |
| "Is this an acceptor or connection?" | `Type` enum | avoids `dynamic_cast` |

The key insight: `FLAG_DEAD` must be in the base class because **Reactor doesn't know about `ConnectionState`** — it only talks to `IEventHandler`. But `ConnectionState` is internal logic that only `ConnectionHandler` itself needs to reason about.
