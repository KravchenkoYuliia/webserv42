Below is a **practical roadmap to implement a streaming HTTP/1.1 request parser yourself**, similar in spirit to parsers used by servers like NGINX or Apache HTTP Server—but simplified enough to build step-by-step.

The goal is a parser that:

* works with **partial TCP data**
* supports **headers**
* supports **Content-Length bodies**
* supports **chunked bodies**
* is **incremental** (`feed()` called multiple times)

---

# Step 0 — Define the parser states

Start with a **minimal FSM**.

```cpp
enum ParseState
{
    REQUEST_LINE,
    HEADERS,
    BODY_IDENTITY,
    BODY_CHUNKED,
    COMPLETE,
    ERROR
};
```

---

# Step 1 — Define the parser class skeleton

Create the structure first before implementing logic.

```cpp
class RequestParser
{
public:
    RequestParser();

    bool feed(const char* data, size_t len);
    bool isComplete() const;

private:
    // FSM
    ParseState state_;

    // raw network buffer
    std::string buffer_;

    // parsed request data
    std::string method_;
    std::string uri_;
    std::string version_;
    std::map<std::string,std::string> headers_;
    std::string body_;

    size_t content_length_;

private:

    // state handlers
    bool parseRequestLine();
    bool parseHeaders();
    bool parseBodyIdentity();
    bool parseBodyChunked();

    // helpers
    bool hasLine() const;
    std::string extractLine();

    bool parseRequestLineFields(const std::string& line);
    bool parseHeaderLine(const std::string& line);
    bool finalizeHeaders();

    size_t findCRLF() const;
};
```

At this stage **nothing is implemented**.

---

# Step 2 — Implement constructor

Initialize the FSM.

```cpp
RequestParser::RequestParser()
: state_(REQUEST_LINE),
  content_length_(0)
{
}
```

---

# Step 3 — Implement the main `feed()` loop

This drives the entire parser.

Prototype:

```cpp
bool feed(const char* data, size_t len);
```

Implementation:

```cpp
bool RequestParser::feed(const char* data, size_t len)
{
    buffer_.append(data, len);

    while (true)
    {
        switch (state_)
        {
            case REQUEST_LINE:
                if (!parseRequestLine())
                    return state_ != ERROR;
                break;

            case HEADERS:
                if (!parseHeaders())
                    return state_ != ERROR;
                break;

            case BODY_IDENTITY:
                if (!parseBodyIdentity())
                    return state_ != ERROR;
                break;

            case BODY_CHUNKED:
                if (!parseBodyChunked())
                    return state_ != ERROR;
                break;

            case COMPLETE:
            case ERROR:
                return state_ != ERROR;
        }
    }
}
```

Important rule:

```
true  = parsing continues
false = need more data OR error
```

The `state_` decides which.

---

# Step 4 — Implement line detection helpers

HTTP headers use:

```
\r\n
```

### Find CRLF

```cpp
size_t RequestParser::findCRLF() const
{
    return buffer_.find("\r\n");
}
```

---

### Check if a full line exists

```cpp
bool RequestParser::hasLine() const
{
    return findCRLF() != std::string::npos;
}
```

---

### Extract a line

```cpp
std::string RequestParser::extractLine()
{
    size_t pos = findCRLF();

    std::string line = buffer_.substr(0, pos);

    buffer_.erase(0, pos + 2);

    return line;
}
```

Now you can parse lines incrementally.

---

# Step 5 — Implement request line parser

HTTP request line:

```
GET /index.html HTTP/1.1
```

### State handler

```cpp
bool RequestParser::parseRequestLine()
{
    if (!hasLine())
        return false;

    std::string line = extractLine();

    if (!parseRequestLineFields(line))
    {
        state_ = ERROR;
        return false;
    }

    state_ = HEADERS;
    return true;
}
```

---

### Field parser

Prototype:

```cpp
bool parseRequestLineFields(const std::string& line);
```

Implementation idea:

```
METHOD SP URI SP VERSION
```

Example result:

```
method_ = "GET"
uri_ = "/index.html"
version_ = "HTTP/1.1"
```

---

# Step 6 — Implement header parser

This is where **tight-loop parsing happens**.

```cpp
bool RequestParser::parseHeaders()
{
    while (hasLine())
    {
        std::string line = extractLine();

        if (line.empty())
        {
            if (!finalizeHeaders())
            {
                state_ = ERROR;
                return false;
            }

            return true;
        }

        if (!parseHeaderLine(line))
        {
            state_ = ERROR;
            return false;
        }
    }

    return false;
}
```

---

# Step 7 — Implement header parsing

Header format:

```
Host: example.com
```

Prototype:

```cpp
bool parseHeaderLine(const std::string& line);
```

Basic logic:

1. find `:`
2. split key/value
3. trim spaces
4. store in `headers_`

---

# Step 8 — Implement `finalizeHeaders()`

This decides **which body mode to use**.

Prototype:

```cpp
bool finalizeHeaders();
```

Logic:

### Case 1 — Content-Length

```
Content-Length: 123
```

```
state_ = BODY_IDENTITY
```

---

### Case 2 — Chunked

```
Transfer-Encoding: chunked
```

```
state_ = BODY_CHUNKED
```

---

### Case 3 — No body

```
state_ = COMPLETE
```

---

# Step 9 — Implement Content-Length body

Prototype:

```cpp
bool parseBodyIdentity();
```

Implementation:

```cpp
bool RequestParser::parseBodyIdentity()
{
    if (buffer_.size() < content_length_)
        return false;

    body_ = buffer_.substr(0, content_length_);

    buffer_.erase(0, content_length_);

    state_ = COMPLETE;

    return true;
}
```

---

# Step 10 — Implement chunked body (simplified)

Prototype:

```cpp
bool parseBodyChunked();
```

Chunk format:

```
HEX_SIZE\r\n
DATA\r\n
...
0\r\n
\r\n
```

Steps:

1. read chunk size line
2. convert hex → integer
3. read that many bytes
4. append to body
5. repeat until size = 0

---

# Step 11 — Add convenience helpers

### Check completion

```cpp
bool RequestParser::isComplete() const
{
    return state_ == COMPLETE;
}
```

---

# Final FSM flow

```
feed()
 ├─ REQUEST_LINE
 ├─ HEADERS
 │   └─ parse all headers in loop
 ├─ BODY_IDENTITY
 │   └─ read Content-Length bytes
 ├─ BODY_CHUNKED
 │   └─ parse chunk stream
 └─ COMPLETE
```

---

# Recommended development order

Follow this exact order to avoid complexity explosions:

1️⃣ request line only
2️⃣ headers only
3️⃣ detect end of headers
4️⃣ support `Content-Length`
5️⃣ support `chunked`
6️⃣ add validation

---

# Final advice

When implementing your parser:

✔ test with **telnet or netcat**

```
nc localhost 8080
```

Then manually type:

```
GET / HTTP/1.1
Host: localhost

```

This is the **best debugging tool**.

---

If you'd like, I can also show you the **3 biggest mistakes people make when writing HTTP parsers** (almost everyone implementing Webserv hits them).
