#ifndef REQUESTPARSER_HPP
#define REQUESTPARSER_HPP

#include "HttpRequest.hpp"
#include <string>

class RequestParser
{
public:
    // ── Parser lifecycle state ────────────────────────────────────────────────
    //
    // Transitions (in order):
    //
    //  PARSE_REQUEST_LINE
    //       │  parseRequestLine() succeeds
    //       ▼
    //  PARSE_HEADERS
    //       │  blank line hit → finalizeHeaders()
    //       ├─── no body needed (GET, DELETE, Content-Length: 0)
    //       │         ▼
    //       │    PARSE_COMPLETE
    //       │
    //       ├─── Transfer-Encoding: chunked
    //       │         ▼
    //       │    PARSE_BODY_CHUNKED
    //       │         │  last chunk (size == 0) received
    //       │         ▼
    //       │    PARSE_COMPLETE
    //       │
    //       └─── Content-Length > 0
    //                 ▼
    //            PARSE_BODY_IDENTITY
    //                 │  bytes_remaining_ == 0
    //                 ▼
    //            PARSE_COMPLETE
    //
    //  Any step can transition to PARSE_ERROR on malformed input.

    enum State {
        PARSE_REQUEST_LINE,
        PARSE_HEADERS,
        PARSE_BODY_IDENTITY,
        PARSE_BODY_CHUNKED,
        PARSE_COMPLETE,
        PARSE_ERROR
    };

    // ── Chunked decoding sub-state ────────────────────────────────────────────
    //
    // Each chunk looks like:
    //   <hex-size>\r\n
    //   <data bytes>\r\n
    //   ...
    //   0\r\n          ← last chunk signals end
    //   \r\n           ← trailing CRLF

    enum ChunkState {
        CHUNK_SIZE,    // reading the hex size line
        CHUNK_DATA,    // reading exactly current_chunk_size_ bytes
        CHUNK_TRAILER  // consuming the \r\n after chunk data
    };

    RequestParser()
        : state_(PARSE_REQUEST_LINE)
        , bytes_remaining_(0)
        , current_chunk_size_(0)
        , chunk_state_(CHUNK_SIZE)
    {}

    // ── Public API ────────────────────────────────────────────────────────────

    // Feed raw bytes from recv() into the parser incrementally.
    // Can be called multiple times as data arrives.
    // Returns false on a parse error — caller should send 400 and close.
    bool feed(const char *data, size_t len);

    bool             isComplete() const { return state_ == PARSE_COMPLETE; }
    bool             hasError()   const { return state_ == PARSE_ERROR; }
    const HttpRequest& getRequest() const { return request_; }

    // Reset for keep-alive: next request on same connection
    void reset()
    {
        state_              = PARSE_REQUEST_LINE;
        bytes_remaining_    = 0;
        current_chunk_size_ = 0;
        chunk_state_        = CHUNK_SIZE;
        raw_buffer_.clear();
        request_.reset();
    }

private:
    // ── Parsing steps ─────────────────────────────────────────────────────────

    bool parseRequestLine(const std::string &line);
    bool parseHeaderLine(const std::string &line);
    bool finalizeHeaders();
    bool parseBodyIdentity();
    bool parseBodyChunked();

    // ── Buffer helpers ────────────────────────────────────────────────────────

    // Returns true if raw_buffer_ contains at least one \r\n
    bool        hasLine()    const;

    // Extracts and removes the first \r\n-terminated line from raw_buffer_
    // (returned string does NOT include the \r\n)
    std::string extractLine();

    // ── Internal state ────────────────────────────────────────────────────────

    State       state_;
    HttpRequest request_;
    std::string raw_buffer_;       // accumulates all recv() bytes

    size_t      bytes_remaining_;  // bytes still needed for identity body
    size_t      current_chunk_size_;
    ChunkState  chunk_state_;
};

#endif
