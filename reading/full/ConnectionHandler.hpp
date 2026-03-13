#ifndef CONNECTIONHANDLER_HPP
#define CONNECTIONHANDLER_HPP

#include "BaseEventHandler.hpp"
#include "RequestParser.hpp"
#include "ResponseBuilder.hpp"
#include <string>

class Reactor;

class ConnectionHandler : public BaseEventHandler
{
public:
    // ── Connection lifecycle state machine ────────────────────────────────────
    //
    // Valid transitions:
    //
    //  STATE_READING
    //      │  parser_.isComplete() → processRequest()
    //      ▼
    //  STATE_WRITING
    //      │  write_buffer_ fully sent
    //      ├── shouldClose() == true  → reactor_.deleteHandler()  [FLAG_DEAD set]
    //      └── shouldClose() == false → STATE_READING (keep-alive)
    //
    //  Any state → STATE_CLOSING on parse error (sends 400 then closes)
    //
    //  FLAG_DEAD (from BaseEventHandler bitmask) is set by reactor_.deleteHandler()
    //  and is checked by Reactor::run() to skip stale events and by
    //  reapDeadHandlers() to free memory.

    enum ConnectionState {
        STATE_READING,  // accumulating request bytes via handleRead()
        STATE_WRITING,  // sending response bytes via handleWrite()
        STATE_CLOSING   // error path: finish sending 400, then close
    };

    ConnectionHandler(int fd, Reactor &reactor, const std::string &root_dir);
    virtual ~ConnectionHandler();

    virtual void handleRead();
    virtual void handleWrite();
    virtual void handleError();
    virtual int  getFd() const;

private:
    // Called when parser_.isComplete():
    //   builds HttpResponse → fills write_buffer_ → arms EPOLLOUT
    void processRequest();

    // Returns true if connection should close after response is sent.
    // True when: HTTP/1.0, or "Connection: close" header present.
    bool shouldClose() const;

    int              fd_;
    Reactor&         reactor_;
    RequestParser    parser_;
    ResponseBuilder  builder_;
    std::string      write_buffer_; // serialized HTTP response pending send()
    size_t           write_offset_; // how many bytes of write_buffer_ already sent
    ConnectionState  state_;        // current lifecycle phase
};

#endif
