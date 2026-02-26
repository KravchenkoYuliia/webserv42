Great project choice — **`webserv`** is one of the most important projects at 42. It forces you to truly understand how the web works under the hood.

Since you don’t want code, I’ll give you a **clear learning roadmap + implementation strategy**, structured like a battle plan.

---

# 🎯 PART 1 — Understand What You Are Building

You are building a simplified version of:

* Nginx
* or Apache HTTP Server

That means:

* TCP server
* HTTP/1.1 protocol handling
* Config file parsing
* Non-blocking I/O
* CGI support
* Multiple clients
* Proper error handling

This is NOT a C++ project.
It is a **networking + system programming project written in C++98**.

---

# 📚 PART 2 — What You Must Learn (In Order)

Follow this order strictly.

---

## 1️⃣ Networking Basics

Learn:

* What is TCP?
* What is a socket?
* What is a port?
* What is a file descriptor?
* Client vs Server
* What is `bind`, `listen`, `accept`?
* What is `recv` / `send`?

📖 Read:

* Beej’s Guide to Network Programming (mandatory)
* man pages:

  * `socket`
  * `bind`
  * `listen`
  * `accept`
  * `recv`
  * `send`
  * `htons`
  * `inet_addr`

Goal:
Be able to build a **simple blocking TCP echo server**.

---

## 2️⃣ Non-Blocking I/O (VERY IMPORTANT)

Your server must handle multiple clients.

Learn:

* Blocking vs Non-blocking sockets
* `fcntl`
* `select`
* `poll`
* `epoll` (Linux)
* `kqueue` (macOS)

For 42:

* mac → `kqueue`
* linux → `epoll`
* or use `poll` (simpler, portable)

📖 Read:

* man `poll`
* man `select`
* man `fcntl`

Goal:

* Build a simple multi-client chat server using `poll`.

If you can do that → you are ready.

---

## 3️⃣ HTTP Protocol (CRITICAL)

You must deeply understand HTTP/1.1.

Learn:

* Request format:

```
GET /index.html HTTP/1.1
Host: localhost:8080
Connection: keep-alive
```

* Response format:

```
HTTP/1.1 200 OK
Content-Type: text/html
Content-Length: 123

<body>...</body>
```

* Methods:

  * GET
  * POST
  * DELETE

* Status codes:

  * 200
  * 201
  * 204
  * 400
  * 403
  * 404
  * 405
  * 500

📖 Read:

* RFC 7230
* RFC 7231
* MDN HTTP documentation

Goal:
Be able to manually type HTTP requests using:

```
telnet localhost 8080
```

---

## 4️⃣ How Nginx Works (Conceptually)

Look at how Nginx structures:

* server blocks
* location blocks
* root
* index
* autoindex
* error_page
* client_max_body_size

Study:

```nginx
server {
    listen 8080;
    server_name localhost;

    location / {
        root ./www;
        index index.html;
    }
}
```

Understand:

* Matching routes
* Longest prefix match
* Config inheritance

---

## 5️⃣ CGI (Common Gateway Interface)

This is usually the hardest part.

Learn:

* What is CGI?
* How `execve` works
* Environment variables
* How to redirect pipes
* How PHP-CGI works

Flow:

Server → fork() → execve() → script → output → server → client

📖 Read:

* man `fork`
* man `execve`
* man `pipe`
* man `dup2`
* CGI specification

Goal:
Be able to execute:

```
php-cgi test.php
```

from your program.

---

# 🏗 PART 3 — Implementation Strategy (Step-by-Step)

DO NOT try to build everything at once.

---

## PHASE 1 — Minimal TCP Server

✔ Create socket
✔ bind
✔ listen
✔ accept
✔ send simple string
✔ close

When this works → move on.

---

## PHASE 2 — Multi-client Server

✔ Switch to non-blocking
✔ Use `poll`
✔ Handle multiple clients
✔ Properly remove closed FDs

At this stage:
Server should handle 100 connections without crashing.

---

## PHASE 3 — Basic HTTP Parsing

✔ Read request into buffer
✔ Detect end of headers (`\r\n\r\n`)
✔ Parse:

* Method
* Path
* Version
* Headers

✔ Return hardcoded response:

```
HTTP/1.1 200 OK
Content-Length: 5

Hello
```

Test with:

* browser
* curl
* telnet

---

## PHASE 4 — Static File Serving

✔ Open file
✔ Read file
✔ Detect content type
✔ Send proper headers
✔ Handle 404

Now you have a basic web server.

---

## PHASE 5 — Config File Parser

✔ Parse config file
✔ Store:

* ports
* server_name
* root
* locations

Use:

* classes
* maps
* vectors

Do NOT parse everything at once.
Start with:

* only `listen`
* only one server

Then extend.

---

## PHASE 6 — Methods

Add support for:

### GET

* Serve file

### POST

* Read body
* Save file or send to CGI

### DELETE

* Delete file
* Return status

---

## PHASE 7 — CGI

✔ Detect file extension (.php, .py, etc.)
✔ fork
✔ create pipes
✔ execve
✔ read output
✔ send to client

Handle:

* Timeout
* Large body
* Errors

---

## PHASE 8 — Edge Cases

✔ Chunked encoding (if required)
✔ Keep-alive connections
✔ Multiple servers (same port)
✔ Error pages
✔ Max body size

---

# 🧠 Architecture Advice

Structure it cleanly:

```
Server
  ├── SocketManager
  ├── Client
  ├── Request
  ├── Response
  ├── Config
  ├── CGIHandler
```

Keep:

* Parsing separate
* Networking separate
* Response building separate

---

# 🔥 Common Mistakes at 42

* Mixing parsing and networking logic
* Not handling partial reads
* Blocking on recv()
* Not handling large POST bodies
* Memory leaks
* Bad FD cleanup
* Not checking return values

---

# 🧪 How To Test Properly

Use:

* curl
* telnet
* browser
* siege (stress test)
* ab (Apache Benchmark)

Test:

* large file
* invalid request
* wrong method
* multiple simultaneous clients
* upload large file
* slow client

---

# 🗓 Suggested Timeline (If Working Seriously)

Week 1 → Networking
Week 2 → poll + HTTP parsing
Week 3 → Static files + config
Week 4 → CGI + polishing

---

# 🧭 If You Feel Lost

Focus on this order:

1. Make socket work
2. Make multi-client work
3. Parse HTTP
4. Serve file
5. Add features slowly

Never jump ahead.

---

# 💬 Final Advice

This project is difficult because:

* It mixes system programming
* Networking
* Parsing
* Architecture design
* C++98 limitations

But once finished, you will truly understand how the web works.

---

If you want, I can next give you:

* 📌 A minimal class architecture blueprint
* 📌 A testing checklist used by evaluators
* 📌 A mental model diagram of how a request flows inside your server
* 📌 Or a “how to split work between 2–3 teammates” guide

What would help you most right now?
