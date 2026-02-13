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

A socket is a software endpoint for communication between two machines.

🧠 More Technical Definition

A socket is defined by:
```
IP Address + Port + Protocol (TCP/UDP)
```

Example:
```
192.168.1.10 : 8080 (TCP)
```

🔥 Important Concept

There are TWO sockets in a TCP server:
- Listening socket
- Connected socket (returned by accept())

### 1.3 What is a port?
📌 Definition

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

## Socket

		Message Passing is a method where processes communicate by sending and receiving messages to exchange data. One process sends a message and the other process receives it, allowing them to share information. Message Passing can be achieved through different methods like Sockets, Message Queues or Pipes.

![message passing](message_passing.png)

Process A  →  Kernel  →  Network stack  →  Internet  →  Kernel  →  Process B
🔥 In the case of a TCP server

When a browser connects to web server:

1️⃣ The browser sends the request to its local kernel.
2️⃣ The kernel initiates the TCP handshake.
3️⃣ The server’s kernel receives the SYN packet.
4️⃣ The server’s kernel notifies your process through accept().
5️⃣ At this point, the connection is established.
