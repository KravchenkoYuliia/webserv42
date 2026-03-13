#include "ConnectionHandler.hpp"
#include "Reactor.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>

ConnectionHandler::ConnectionHandler(int fd, Reactor &reactor,
                                     const std::string &root_dir)
    : BaseEventHandler(BaseEventHandler::CONNECTION)
    , fd_(fd)
    , reactor_(reactor)
    , builder_(root_dir)
    , write_offset_(0)
    , state_(STATE_READING)
{}

ConnectionHandler::~ConnectionHandler()
{
    if (fd_ != -1)
        close(fd_);
}

int ConnectionHandler::getFd() const { return fd_; }

// ── handleRead ────────────────────────────────────────────────────────────────
//
// Called by Reactor::run() on EPOLLIN.
//
// State guard: only acts in STATE_READING.
//   If we're in STATE_WRITING, a stale EPOLLIN fired after we already have a
//   complete request — safe to ignore, the write will finish on its own.
//
// Drains the socket in a loop because:
//   - The fd is non-blocking → recv() returns EAGAIN when empty
//   - One EPOLLIN event may carry multiple chunks (level-triggered epoll)
//
// On each recv():
//   - n == 0            → client closed connection (EOF)
//   - n < 0 + EAGAIN    → no more data right now, wait for next EPOLLIN
//   - n < 0 + other     → hard error, close connection
//   - n > 0             → feed bytes to parser
//       → parse error   → send 400, transition to STATE_CLOSING
//       → complete      → processRequest()

void ConnectionHandler::handleRead()
{
    // state guard — ignore EPOLLIN while we are sending a response
    if (state_ != STATE_READING)
        return;

    char buf[4096];

    while (true)
    {
        ssize_t n = recv(fd_, buf, sizeof(buf), 0);

        if (n == 0) // EOF — client closed connection
        {
            reactor_.deleteHandler(fd_); // sets FLAG_DEAD
            return;
        }

        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break; // socket drained — wait for next EPOLLIN
            reactor_.deleteHandler(fd_);
            return;
        }

        if (!parser_.feed(buf, static_cast<size_t>(n)))
        {
            // malformed request — respond with 400 then close
            HttpRequest  dummy;
            HttpResponse err;
            err.setStatus(400, "Bad Request");
            err.setHeader("Content-Type", "text/html");
            err.setHeader("Connection", "close");
            err.setBody("<html><body><h1>400 Bad Request</h1></body></html>");

            write_buffer_ = err.serialize();
            write_offset_ = 0;

            state_ = STATE_CLOSING;        // state machine: error path
            reactor_.enableWrite(this);    // bitmask: arm EPOLLOUT to flush 400
            return;
        }

        if (parser_.isComplete())
        {
            processRequest();
            return;
        }
    }
}

// ── processRequest ────────────────────────────────────────────────────────────
//
// Called when parser_.isComplete().
// Builds the HTTP response, stores it in write_buffer_, arms EPOLLOUT.
//
// After this call:
//   state_        == STATE_WRITING
//   FLAG_WRITABLE == true (EPOLLOUT registered in epoll)
//   write_offset_ == 0

void ConnectionHandler::processRequest()
{
    HttpRequest  request  = parser_.getRequest();
    HttpResponse response = builder_.build(request);

    // propagate Connection: close from request to response if needed
    if (shouldClose())
        response.setClose();

    write_buffer_ = response.serialize();
    write_offset_ = 0;

    state_ = STATE_WRITING;        // state machine: advance to write phase
    reactor_.enableWrite(this);    // bitmask: register EPOLLOUT
}

// ── handleWrite ───────────────────────────────────────────────────────────────
//
// Called by Reactor::run() on EPOLLOUT.
//
// State guard: only acts in STATE_WRITING or STATE_CLOSING.
//   STATE_WRITING → normal response send, keep-alive eligible
//   STATE_CLOSING → error response send (400), always closes after
//
// Sends as much of write_buffer_ as the kernel will accept:
//   - EAGAIN → kernel send buffer full, wait for next EPOLLOUT
//   - error  → close immediately
//   - fully drained → disarm EPOLLOUT (avoid busy-loop)
//       → STATE_CLOSING or shouldClose() → deleteHandler (FLAG_DEAD)
//       → otherwise → STATE_READING + parser_.reset() (keep-alive)

void ConnectionHandler::handleWrite()
{
    // state guard — should never fire outside write phases
    if (state_ != STATE_WRITING && state_ != STATE_CLOSING)
        return;

    while (write_offset_ < write_buffer_.size())
    {
        ssize_t n = send(fd_,
                         write_buffer_.c_str() + write_offset_,
                         write_buffer_.size()  - write_offset_,
                         0);
        if (n < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return; // kernel buffer full — wait for next EPOLLOUT
            reactor_.deleteHandler(fd_);
            return;
        }
        write_offset_ += static_cast<size_t>(n);
    }

    // ── buffer fully sent ─────────────────────────────────────────────────────

    reactor_.disableWrite(this); // bitmask: remove EPOLLOUT — avoid busy-loop

    write_buffer_.clear();
    write_offset_ = 0;

    if (state_ == STATE_CLOSING || shouldClose())
    {
        // state machine: error path or Connection: close → terminate
        reactor_.deleteHandler(fd_); // sets FLAG_DEAD
        return;
    }

    // state machine: keep-alive → reset for next request on same connection
    state_ = STATE_READING;
    parser_.reset();
}

// ── handleError ───────────────────────────────────────────────────────────────
//
// Called by Reactor::run() on EPOLLERR | EPOLLHUP.
// State doesn't matter — hard kernel error always terminates the connection.

void ConnectionHandler::handleError()
{
    reactor_.deleteHandler(fd_); // sets FLAG_DEAD regardless of state_
}

// ── shouldClose ───────────────────────────────────────────────────────────────
//
// Returns true if the connection must close after the current response:
//   - HTTP/1.0 has no keep-alive by default
//   - Explicit "Connection: close" header from client

bool ConnectionHandler::shouldClose() const
{
    std::string conn = parser_.getRequest().getHeader("connection");
    std::string ver  = parser_.getRequest().getVersion();
    return (ver == "HTTP/1.0" || conn == "close");
}
