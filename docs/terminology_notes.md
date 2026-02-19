# Terminology Notes

## 0 - OSI Model

The `OSI Model` is a conceptual model to organize networking into layers.

It has 7 layers:
1. Physical
2. Data Link
3. Network
4. Transport
5. Session
6. Presentation
7. Application

for `Webserv`, a simplified representation of the OSI Model would be:

```
Application   → HTTP
Transport     → TCP
Network       → IP
Link          → Ethernet / Wi-Fi
```

Which means, we mainly care about:

🔹 Application Layer
- HTTP request format
- HTTP response format
- Headers
- Status codes
- Methods (GET, POST, DELETE)

🔹 Transport Layer
- TCP
- Ports
- Connections
- Listening socket
- accept()
- recv()
- send()

## 1 - Networking basics

### 1.1 What is TCP?
📌 Definition

TCP (Transmission Control Protocol) is a transport-layer protocol that enables reliable, ordered, and error-checked communication between two devices over a network.

🧠 What makes TCP special?

TCP guarantees:
- ✅ Reliable delivery (no data loss)
- ✅ Ordered data (correct sequence)
- ✅ Error checking
- ✅ Flow control
- ✅ Congestion control

Unlike UDP, TCP is connection-oriented.

🔄 How TCP Works (Simplified)

1️⃣ Three-Way Handshake [(see geekforgeek article for more details)](https://www.geeksforgeeks.org/computer-networks/tcp-3-way-handshake-process/)

Before data is sent:
1. Client → SYN
2. Server → SYN-ACK
3. Client → ACK

Connection established.

2️⃣ Data Transfer

Data is split into segments and sent reliably.

3️⃣ Connection Close

Connection is gracefully terminated.

### 1.2 What is a socket?
📌 Definition

A socket is a software abstraction that represents one endpoint of a network communication.

🧠 More Technical Definition

A socket is defined by:
```
IP Address + Port + Protocol (TCP/UDP)
```

Example:
```
192.168.1.10 : 8080 (TCP)
```

🔥 Important Concept (TCP Server)

A TCP server uses:
- One listening socket
- One connected socket per client (returned by accept())

If multiple clients connect, the server will have multiple connected sockets.

A TCP server has:

1️⃣ One listening socket
- Bound to a port
- Waits for connections

2️⃣ One new connected socket per client
- Returned by accept()
- Used for communication with that specific client

So if 5 clients connect:
- 1 listening socket
- 5 connected sockets

=> Total: 6 sockets

### 1.3 What is an Interface: Port Pair?

#### 1.3.1 What is a port?
📌 Definition

Port is a 16-bits number ( 0-65535 )

A port is a numerical identifier used to distinguish different services on the same machine.

It allows one computer to run multiple network services simultaneously.

🧠 Why Ports Exist

Your computer can run:
- Web server
- SSH server
- Database
- FTP server
- etc.

All on the same IP.

Ports tell the OS which application should receive the data.

Ports from 0 to 1023 requires root privilege
We can use ports 1024-65535 in config file

#### 1.3.2 What is an Interface?
📌 Definition
An interface is basically an IP address on your machine.

Common ones:
| Interface     | Meaning                                       |
| ------------- | --------------------------------------------- |
| `127.0.0.1`   | localhost (only accessible from your machine) |
| `0.0.0.0`     | all interfaces                                |
| `192.168.x.x` | local network IP                              |
| Public IP     | accessible from the internet                  |

#### 1.3.3 What is an Interface: Port Pair?
It’s simply: `IP_ADDRESS:PORT`

Examples:
- 127.0.0.1:8080
- 0.0.0.0:80
- 192.168.1.10:3000

Each one is a separate listening socket.

### 1.4 Client - Server Architecture
📌 Definition

Client–Server is a network model where:
- Client → requests a service
- Server → provides the service

🧠 What Actually Happens

1. Client creates a socket
2. Client connects to server IP + port
3. Server accepts connection
4. Client sends HTTP request
5. Server sends HTTP response
6. Connection closes (or stays open)

### 1.5 I/O Multiplexing: `select()` VS `poll()` VS `epoll()`


this is **core event-driven server knowledge**, and it directly affects how you design your TCP server event loop.
#### 1️⃣ Why do `select()`, `poll()`, and `epoll()` exist?

When you build a server:

* You may have **1000+ client sockets**
* You cannot block on one client
* You need to know **which socket is ready**

These functions are called **I/O multiplexing mechanisms**.

They allow a process to:

> Block until one or more file descriptors become ready for I/O.

Instead of handling one socket per thread, you can manage many sockets in a single event loop.
📌 `I/O Multiplexing` Definition

I/O multiplexing allows us to simultaneously monitor multiple file descriptors to
see if I/O is possible on any of them.

#### 2️⃣ What is `select()`?
📌 Definition

`select()` is a system call that monitors multiple file descriptors to see if they are:

* Ready for reading
* Ready for writing
* In an exceptional state (errors)

🧠 How it Works

1. Put file descriptors into sets:
   * `readfds`
   * `writefds`
   * `exceptfds`
2. Call `select()`
3. The kernel blocks until at least one FD is ready (or timeout occurs)
4. The sets are modified to indicate which FDs are ready

❗ Limitations

* Limited by `FD_SETSIZE` (commonly ~1024 files)
* O(n) complexity — Scans all FDs every time
* You must rebuild fd_set every loop
* Inefficient for large numbers of connections

#### 3️⃣ What is `poll()`?

📌 Definition

`poll()` is similar to `select()` but:

* Uses an array of `struct pollfd`
* Does not have a fixed hard limit like `FD_SETSIZE`
* Has a cleaner and more flexible interface

🧠 How it Works

You create an array:

```cpp
struct pollfd fds[];
```

Each entry contains:
* fd (file descriptor)
* events (what you care about)
* revents (what actually happened)

You call `poll()`, and the kernel fills in which FDs are ready.

❗ Limitation

Still:

* Still O(n) — kernel scans all FDs on each call
* Performance degrades linearly with number of connections

`poll()` improves usability over `select()`, but scalability is still limited.

#### 4️⃣ What is `epoll()`? (Linux only)
📌 Definition

`epoll()` is a high-performance I/O event notification system for Linux.

Designed for:

> Massive concurrency (10k–1M connections)

🧠 Key Idea

With `select()` and `poll()`:

> Each call scans the entire set of file descriptors.

With `epoll()`:
* You register file descriptors once
* The kernel maintains an internal interest list
* The kernel tracks which FDs become ready
* `epoll_wait()` returns only the ready file descriptors

🚀 Why It Scales Better

* Adding/removing FDs is O(1)
* Waiting is proportional to the number of ready FDs
* No need to rescan the entire FD set
* No hard descriptor limit (other than system limits)

This makes it much more efficient for high-concurrency servers.

## 1.6 I/O multiplexing mechanisms

## NGINX


## HTTP Protocol
## `URL` = Uniform Resource Locators
	is the address that specifies where a resource is located on the internet and how to access it

http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
If the port is empty or not given, port 80 is assumed.

## Method `GET`
	retrieves a resource without modifying the server state
	[Give me the resource]

## Method `POST`
	sends data to the server to create or modify a resource
	[Do smth with this resource]

## Method `DELETE`
	requests the server to remove a specified resource
	[delete this resource]


## request-response

- google "flowers"
- browser=client ( example: Mozilla ) creates a HTTP request and sends it to Google=host=origin server
	example:
			GET /search?q=flowers HTTP/1.1
			Host: www.google.com

	example:

			GET /hello.txt HTTP/1.1
			User-Agent: curl/7.64.1
			Host: www.example.com
			Accept-Language: en, mi



- host listens for request -> receives a request -> parses it -> executes internal logique( algorithms, data base, indices ) -> creates a HTTP response
	example:
			HTTP/1.1 200 OK
			Content-Type: text/html
			Content-Length: 12345

			<html> ... result ... </html>



### to see a real http request
		```bash
		python3 -m http.server 8080 &
		curl -v http://localhost:8080/
		```

		or
- connect to nginx
- `nc localhost 8080`
- write a request
```
GET / HTTP/1.1
Host: localhost
```



## Socket

		Message Passing is a method where processes communicate by sending and receiving messages to exchange data. One process sends a message and the other process receives it, allowing them to share information. Message Passing can be achieved through different methods like Sockets, Message Queues or Pipes.

![message passing](./assets/message_passing.png)

Process A  →  Kernel  →  Network stack  →  Internet  →  Kernel  →  Process B
🔥 In the case of a TCP server

When a browser connects to web server:

- 1️⃣ The browser sends the request to its local kernel.
- 2️⃣ The kernel initiates the TCP handshake.
- 3️⃣ The server’s kernel receives the SYN packet.
- 4️⃣ The server’s kernel notifies your process through accept().
- 5️⃣ At this point, the connection is established.




## Test nginx and telnet
- create docker with nginx
- run it with port 8080
	`docker run --rm -p 8080:80 nginx`

- connect via telnet
	`telnet localhost 8080`
- send a request

```
GET / HTTP/1.1
Host: localhost
```