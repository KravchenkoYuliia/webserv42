#include "RequestParser.hpp"
#include <sstream>
#include <cstdlib>   // strtoul
#include <cctype>    // tolower
#include <algorithm> // std::transform

// ── Public: feed ─────────────────────────────────────────────────────────────
//
// Called every time recv() returns bytes. Appends to raw_buffer_ then drives
// the state machine until it either blocks waiting for more data or completes.
//
// Example flow for "GET / HTTP/1.1\r\nHost: x\r\n\r\n":
//
//   feed("GET / HTTP/1.1\r\nHo") → appends, extracts request line, partial header buffered
//   feed("st: x\r\n\r\n")        → extracts header line, blank line → PARSE_COMPLETE

bool RequestParser::feed(const char *data, size_t len)
{
    raw_buffer_.append(data, len);

    while (true)
    {
        switch (state_)
        {
            case PARSE_REQUEST_LINE:
            {
                if (!hasLine())
                    return true; // need more data

                std::string line = extractLine();
                if (!parseRequestLine(line))
                {
                    state_ = PARSE_ERROR;
                    return false;
                }
                state_ = PARSE_HEADERS;
                break;
            }

            case PARSE_HEADERS:
            {
                if (!hasLine())
                    return true; // need more data

                std::string line = extractLine();

                if (line.empty()) // blank line → end of headers
                {
                    if (!finalizeHeaders())
                    {
                        state_ = PARSE_ERROR;
                        return false;
                    }
                    // finalizeHeaders sets the next state
                    break;
                }

                if (!parseHeaderLine(line))
                {
                    state_ = PARSE_ERROR;
                    return false;
                }
                break;
            }

            case PARSE_BODY_IDENTITY:
            {
                if (!parseBodyIdentity())
                {
                    state_ = PARSE_ERROR;
                    return false;
                }
                // parseBodyIdentity sets state_ to PARSE_COMPLETE when done
                break;
            }

            case PARSE_BODY_CHUNKED:
            {
                if (!parseBodyChunked())
                {
                    state_ = PARSE_ERROR;
                    return false;
                }
                // parseBodyChunked sets state_ to PARSE_COMPLETE on last chunk
                break;
            }

            case PARSE_COMPLETE:
            case PARSE_ERROR:
                return state_ != PARSE_ERROR;
        }

        // If we just reached COMPLETE or are blocked waiting for data, stop
        if (state_ == PARSE_COMPLETE || state_ == PARSE_ERROR)
            break;
    }

    return state_ != PARSE_ERROR;
}

// ── parseRequestLine ──────────────────────────────────────────────────────────
//
// Parses: "GET /index.html HTTP/1.1"
//          ^^^  ^^^^^^^^^^^  ^^^^^^^
//         method   uri       version

bool RequestParser::parseRequestLine(const std::string &line)
{
    std::istringstream iss(line);
    std::string method_str, uri, version;

    if (!(iss >> method_str >> uri >> version))
        return false; // malformed — missing tokens

    // method
    if      (method_str == "GET")    request_.setMethod(HttpRequest::GET);
    else if (method_str == "POST")   request_.setMethod(HttpRequest::POST);
    else if (method_str == "DELETE") request_.setMethod(HttpRequest::DELETE);
    else                             request_.setMethod(HttpRequest::UNKNOWN);

    // basic URI validation — must start with '/'
    if (uri.empty() || uri[0] != '/')
        return false;

    // version check — we only support HTTP/1.0 and HTTP/1.1
    if (version != "HTTP/1.1" && version != "HTTP/1.0")
        return false;

    request_.setUri(uri);
    request_.setVersion(version);
    return true;
}

// ── parseHeaderLine ───────────────────────────────────────────────────────────
//
// Parses: "Content-Type: text/html"
//          ^^^^^^^^^^^^  ^^^^^^^^^
//              key          value
//
// Keys are normalized to lowercase per RFC 7230.
// Leading/trailing whitespace is stripped from values.

bool RequestParser::parseHeaderLine(const std::string &line)
{
    size_t colon = line.find(':');
    if (colon == std::string::npos)
        return false; // malformed header — no colon

    std::string key   = line.substr(0, colon);
    std::string value = line.substr(colon + 1);

    // normalize key to lowercase
    for (size_t i = 0; i < key.size(); ++i)
        key[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(key[i])));

    // strip leading whitespace from value
    size_t start = value.find_first_not_of(" \t");
    if (start != std::string::npos)
        value = value.substr(start);

    // strip trailing whitespace from value
    size_t end = value.find_last_not_of(" \t\r");
    if (end != std::string::npos)
        value = value.substr(0, end + 1);

    request_.setHeader(key, value);
    return true;
}

// ── finalizeHeaders ───────────────────────────────────────────────────────────
//
// Called when blank line is found (end of headers).
// Decides which body parsing mode to use (if any) and sets the next state.
//
// Priority (per RFC 7230 §3.3):
//   1. Transfer-Encoding: chunked  → chunked body
//   2. Content-Length: N           → identity body of exactly N bytes
//   3. neither                     → no body (GET / DELETE)

bool RequestParser::finalizeHeaders()
{
    std::string te = request_.getHeader("transfer-encoding");
    std::string cl = request_.getHeader("content-length");
    std::string ct = request_.getHeader("content-type");

    // detect chunked
    if (te == "chunked")
    {
        request_.setChunked(true);
        chunk_state_ = CHUNK_SIZE;
        state_ = PARSE_BODY_CHUNKED;
        return true;
    }

    // detect multipart (content-type: multipart/form-data; boundary=...)
    if (ct.find("multipart/form-data") != std::string::npos)
    {
        size_t b = ct.find("boundary=");
        if (b == std::string::npos)
            return false; // multipart without boundary is malformed
        request_.setMultipart(true);
        request_.setMultipartBoundary(ct.substr(b + 9));
    }

    // detect identity body
    if (!cl.empty())
    {
        char *end;
        unsigned long n = std::strtoul(cl.c_str(), &end, 10);
        if (*end != '\0')
            return false; // non-numeric Content-Length

        request_.setContentLength(n);

        if (n == 0)
        {
            state_ = PARSE_COMPLETE; // no body to read
            return true;
        }

        bytes_remaining_ = n;
        state_ = PARSE_BODY_IDENTITY;
        return true;
    }

    // no body (GET, DELETE, etc.)
    state_ = PARSE_COMPLETE;
    return true;
}

// ── parseBodyIdentity ─────────────────────────────────────────────────────────
//
// Reads exactly Content-Length bytes from raw_buffer_.
// Called repeatedly by feed() until bytes_remaining_ reaches 0.
//
// Example: Content-Length: 5, body arrives in two feed() calls
//   feed("He")  → appends 2 bytes, bytes_remaining_ = 3
//   feed("llo") → appends 3 bytes, bytes_remaining_ = 0 → PARSE_COMPLETE

bool RequestParser::parseBodyIdentity()
{
    if (raw_buffer_.empty())
        return true; // wait for more data — not an error

    size_t to_consume = std::min(raw_buffer_.size(), bytes_remaining_);

    request_.appendBody(raw_buffer_.substr(0, to_consume));
    raw_buffer_.erase(0, to_consume);
    bytes_remaining_ -= to_consume;

    if (bytes_remaining_ == 0)
        state_ = PARSE_COMPLETE;

    return true;
}

// ── parseBodyChunked ──────────────────────────────────────────────────────────
//
// Implements RFC 7230 §4.1 chunked transfer decoding.
//
// Wire format:
//   7\r\n           ← chunk-size in hex
//   Mozilla\r\n     ← chunk-data (exactly 7 bytes) + CRLF
//   9\r\n
//   Developer\r\n
//   0\r\n           ← last-chunk (size = 0)
//   \r\n            ← trailing CRLF
//
// Sub-states (chunk_state_):
//   CHUNK_SIZE    → read hex line → set current_chunk_size_
//   CHUNK_DATA    → read exactly current_chunk_size_ bytes
//   CHUNK_TRAILER → consume \r\n after data, loop or complete

bool RequestParser::parseBodyChunked()
{
    while (true)
    {
        switch (chunk_state_)
        {
            case CHUNK_SIZE:
            {
                if (!hasLine())
                    return true; // wait for more data

                std::string size_line = extractLine();

                // strip any chunk extensions (";ext=value" after size)
                size_t semi = size_line.find(';');
                if (semi != std::string::npos)
                    size_line = size_line.substr(0, semi);

                char *end;
                unsigned long chunk_size = std::strtoul(size_line.c_str(), &end, 16);
                if (*end != '\0' && *end != '\r')
                    return false; // malformed chunk size

                current_chunk_size_ = chunk_size;

                if (current_chunk_size_ == 0)
                {
                    // last chunk — consume trailing \r\n then complete
                    chunk_state_ = CHUNK_TRAILER;
                    break;
                }

                chunk_state_ = CHUNK_DATA;
                break;
            }

            case CHUNK_DATA:
            {
                if (raw_buffer_.size() < current_chunk_size_)
                    return true; // wait — full chunk not yet arrived

                request_.appendBody(raw_buffer_.substr(0, current_chunk_size_));
                raw_buffer_.erase(0, current_chunk_size_);
                current_chunk_size_ = 0;
                chunk_state_ = CHUNK_TRAILER;
                break;
            }

            case CHUNK_TRAILER:
            {
                // consume the \r\n that follows chunk data (or terminates last chunk)
                if (raw_buffer_.size() < 2)
                    return true; // wait

                if (raw_buffer_[0] != '\r' || raw_buffer_[1] != '\n')
                    return false; // protocol violation

                raw_buffer_.erase(0, 2);

                if (current_chunk_size_ == 0)
                {
                    // we just consumed the trailer after size=0 → done
                    state_ = PARSE_COMPLETE;
                    return true;
                }

                // more chunks to come
                chunk_state_ = CHUNK_SIZE;
                break;
            }
        }

        if (state_ == PARSE_COMPLETE)
            return true;
    }
}

// ── hasLine ───────────────────────────────────────────────────────────────────
//
// Returns true if raw_buffer_ contains at least one complete \r\n sequence.
// Used to avoid calling extractLine() when no full line is available yet.

bool RequestParser::hasLine() const
{
    return raw_buffer_.find("\r\n") != std::string::npos;
}

// ── extractLine ───────────────────────────────────────────────────────────────
//
// Removes and returns the first \r\n-terminated line from raw_buffer_.
// The returned string does NOT include the \r\n.
//
// Example:
//   raw_buffer_ = "GET / HTTP/1.1\r\nHost: x\r\n"
//   extractLine() → "GET / HTTP/1.1"
//   raw_buffer_ → "Host: x\r\n"

std::string RequestParser::extractLine()
{
    size_t pos = raw_buffer_.find("\r\n");
    if (pos == std::string::npos)
        return ""; // caller must check hasLine() first

    std::string line = raw_buffer_.substr(0, pos);
    raw_buffer_.erase(0, pos + 2); // remove line + \r\n
    return line;
}
