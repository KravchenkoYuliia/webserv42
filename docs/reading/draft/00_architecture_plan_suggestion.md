# Question

can you suggest an architecture project plan for webserv which use an epoll approach?
then can you suggest how to split the work for a group of 2 people?


# Answer

Excellent. Now we’re at real system design level 🔥
I’ll give you:

1️⃣ A clean **epoll-based architecture plan**
2️⃣ A realistic **project roadmap (phases)**
3️⃣ A smart **work split for 2 people**

Everything tailored for 42 Webserv (mandatory part).

---

# 🧠 PART 1 — High-Level Architecture (epoll-based)

You are building an **event-driven, non-blocking server**.

So the architecture must revolve around:

```
epoll
```

NOT per-connection threads.

---

# 🏗 Core Architecture Overview

```
                ┌────────────────────┐
                │     ConfigParser   │
                └─────────┬──────────┘
                          ↓
                ┌────────────────────┐
                │   ServerManager    │
                │ (create sockets)   │
                └─────────┬──────────┘
                          ↓
                ┌────────────────────┐
                │   EventLoop (epoll)│
                └─────────┬──────────┘
                          ↓
                ┌────────────────────┐
                │  ConnectionManager │
                └─────────┬──────────┘
                          ↓
                ┌────────────────────┐
                │   RequestHandler   │
                └─────────┬──────────┘
                          ↓
                ┌────────────────────┐
                │   ResponseBuilder  │
                └────────────────────┘
```

---

# 🔥 Core Principle

Everything revolves around:

```
epoll_wait()
```

You never block.

All sockets are:

```
fcntl(fd, F_SETFL, O_NONBLOCK)
```

---

# 🧩 Core Components

---

## 1️⃣ ConfigParser

Responsibility:

* Parse config file
* Build:

  * vector<Server>
  * grouped by listen IP:port

No networking here.

---

## 2️⃣ ServerManager

After config parsed:

For each unique `(IP, port)`:

* socket()
* bind()
* listen()
* set non-blocking
* add to epoll as EPOLLIN

Store mapping:

```
listen_fd → vector<Server*>
```

---

## 3️⃣ EventLoop (Epoll Core)

This is your heart:

```cpp
while (true)
{
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++)
    {
        if (events[i] is listening socket)
            accept new connection;
        else
            handle client socket;
    }
}
```

---

## 4️⃣ Connection Object

Each client connection needs state:

```cpp
class Connection {
public:
    int fd;
    std::string read_buffer;
    std::string write_buffer;
    bool request_complete;
    HttpRequest request;
};
```

Store in:

```cpp
std::map<int, Connection>
```

Key = client fd.

---

## 5️⃣ Request Lifecycle in epoll Model

### When EPOLLIN:

* read()
* append to buffer
* try parse HTTP
* if request complete:

  * generate response
  * switch socket to EPOLLOUT

---

### When EPOLLOUT:

* write()
* remove written bytes
* if fully sent:

  * close connection (or keep-alive if implemented)

---

# 🛠 PART 2 — Project Roadmap (Phases)

This is critical to avoid chaos.

---

## ✅ PHASE 1 — Config Parsing Only

* Parse server blocks
* Parse location blocks
* Validate config
* No networking yet

Test with:

```
./webserv config.conf
```

Print parsed structure.

---

## ✅ PHASE 2 — Basic epoll Server

* One listen socket
* Accept connections
* Echo back simple HTTP 200

No routing yet.

---

## ✅ PHASE 3 — HTTP Parsing

* Parse:

  * Request line
  * Headers
  * Content-Length
* Detect end of headers (`\r\n\r\n`)
* Support GET only first

---

## ✅ PHASE 4 — Server & Location Selection

* Implement:

  * server selection
  * default fallback
  * longest prefix match

---

## ✅ PHASE 5 — Static File Serving

* root resolution
* file exists?
* directory logic
* index handling
* autoindex
* correct status codes

---

## ✅ PHASE 6 — POST + Body Handling

* respect client_max_body_size
* read body fully
* multipart parsing
* file saving

---

## ✅ PHASE 7 — Error Pages

* error_page internal redirect

---

## ✅ PHASE 8 — Polish & Edge Cases

* trailing slash redirect
* 405 with Allow header
* 413
* 400 handling
* memory cleanup
* valgrind clean

---

# 👥 PART 3 — How To Split Work (2 People)

The key rule:

⚠️ Do NOT split randomly by features.
Split by **layer**.

---

# 💡 Best Split Strategy

## 👤 Person A — Networking & epoll Core

Responsible for:

* ServerManager
* epoll loop
* Connection class
* Non-blocking IO
* read/write handling
* socket lifecycle
* keep-alive handling
* clean shutdown

This person builds the engine.

---

## 👤 Person B — HTTP & Routing Logic

Responsible for:

* HTTP parsing
* Server selection
* Location matching
* Filesystem resolution
* Directory logic
* Autoindex HTML generation
* Error page handling
* POST & multipart parsing

This person builds the brain.

---

# 🤝 Integration Point

The integration boundary:

```
Connection has full HttpRequest
↓
Call RequestHandler
↓
Get HttpResponse
↓
Connection handles sending
```

Define this interface early:

```cpp
HttpResponse handleRequest(HttpRequest&, SocketInfo&);
```

This avoids merge chaos.

---

# 🧠 Why This Split Works

Because:

* Networking and epoll logic is very different mental domain
* HTTP + routing logic is separate complexity
* Both can work mostly independently
* Fewer merge conflicts

---

# ⚠️ What NOT To Do

❌ Both touching epoll
❌ Both editing Connection class
❌ Both modifying event loop
❌ Mixing parsing with read()

---

# 🎯 Final Advice For 42

Evaluators care about:

* Clean epoll loop
* No blocking calls
* Correct location priority
* Correct directory logic
* Proper error codes
* Clean architecture

They do NOT care about:

* Fancy templates
* Overengineering
* Micro-optimizations

---

# 🚀 If You Want Next

I can:

* Show you a minimal epoll loop skeleton ready to paste
* Or show you a clean directory tree structure for the project
* Or simulate a full request lifecycle through epoll

You’re designing this the right way. This is exactly how to succeed in Webserv 🔥
