# Implementation Plan

## Architecture plan
webserv/
│
├── Makefile
├── config/
│   └── default.conf
│
├── include/
│   ├── core/
│   │   ├── ServerManager.hpp
│   │   ├── EventLoop.hpp
│   │   ├── Connection.hpp
│   │   ├── ConnectionManager.hpp
│   │   └── Socket.hpp
│   │
│   ├── config/
│   │   ├── ConfigParser.hpp
│   │   ├── ServerConfig.hpp
│   │   ├── LocationConfig.hpp
│   │   └── Tokenizer.hpp
│   │
│   ├── http/
│   │   ├── HttpRequest.hpp
│   │   ├── HttpResponse.hpp
│   │   ├── RequestParser.hpp
│   │   └── ResponseBuilder.hpp
│   │
│   ├── routing/
│   │   ├── Router.hpp
│   │   └── LocationMatcher.hpp
│   │
│   └── utils/
│       ├── Logger.hpp
│       ├── FileUtils.hpp
│       └── StringUtils.hpp
│
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   ├── config/
│   ├── http/
│   ├── routing/
│   └── utils/
│
└── www/
    ├── default/
    └── test_site/

## Config file Definition
- [] Multiple Server
- [] listen (port + host)
- [] server_name
- [] root
- [] client_max_body_size
- [] error_page
- [] index
- [] allowed_methods
- [] autoindex
- [] upload directory (if required)
- [] rule longest prefix match priority

## Core
### ServerManager
- [] load server config
- [] set up server config (create / bind / listen)
- [] start server (start even loop and register listening socket in EvenLoop?)
- []

### EvenLoop
This class is responsible to encapsulates epoll.
- [] add fd
- [] remove fd
- [] modify fd
- [] run(connectionManager) => infinite loop that is responsible to wait for events and dispatch them to the right owner through connection_manager

### Connection
Represent one client
- [] handle connection lifecycle (reading, writing, closed)

```
Because one connection goes through phases:

Client connects
    ↓
Reading request
    ↓
Building response
    ↓
Writing response
    ↓
Either:
    - Close
    - Or go back to READING (keep-alive)

Without a state:
- You don’t know if EPOLLIN should be processed
- You don’t know if EPOLLOUT should be processed
- You don’t know if the connection is finished
```

- [] read from socket => handleRead()
- [] append buffer
- [] handleWrite()
    - [] send()? be carefull to check that the entire response have been send
- [] isRequestComplete()
- [] isResponseSent()
- [] close connection

### ConnectionManager
This class manages client connections.
- [] map fd <-> connection
- [] add listening Socket
- [] is listening socket
- [] handleNewConnection
- [] handleClientEvent
- [] removeConnection
- [] forward EPOLLIN to connection
- [] forward EPOLLOUT to connection
- [] remove connection from the list

### Socket
This class is responsible to wrap raw socket API.
- [] Create socket
- [] Bind socket
- [] Listen socket
- [] accept client socket
- [] set non blocking socket
- [] getter to retrieve listening fd
- [] allow reuse of socket => wrap method setsockopt()

## Config
### ConfigParser
- [] Parse method that will englob parserServer, parseLocation, etc.
- [] Parse server block
- [] Parse location block
- [] validate required directives block
- [] detect syntax errors

### ServerConfig
Class is responsible to hold all the server config attributes
- [] port
- [] host
- [] root
- [] server_name
- [] client_max_body_size
- [] locations_list
- [] error_pages ? not sure

### LocationConfig
Class is responsible to hold all the location config attributes
- [] path
- [] root
- [] allowed_methods
- [] auto_index
- [] index
- [] redirect
- [] cgi?

### Tokenizer
Class is responsible to convert config file into tokens.
- [] skip comment lines
- []
- []



## HTTP
### HttpRequest
Structured representation of a parsed HTTP request.
- [] method
- [] uri
- [] version
- [] headers map
- [] body
- [] qury string
- [] parsed path

### HttpResponse
Structured representation of a parsed HTTP request.
- [] status code
- [] reason phrase
- [] headers
- [] body


### Request Parser
Class is responsible to extracts data from readBuffer
- [] Parse HTTP headers
- [] Parse body
- [] Detect end of request `\r\n\r\n`
- [] Parse Methods
- [] Parse URI
- [] Parse HTTP version
- [] Detect malformed syntax
- [] support partial reads
- [] Return parse result (INCOMPLETE / COMPLETE / ERROR)

Potential edge cases to handle:
- [] Malformed headers
- [] Multiple Content-Length
- [] No Content-Length -> recv()
- [] Chunked encoding
- [] Header too large
- [] Missing CRLF
- [] Invalid HTTP version
    => accept all HTTP version
    => or return 505 HTTP Version Not Supported

### Response Builder
Build HttpResponse based on:
- Request
- Matched location
- Filesystem

things to implement:
- []
- []
- []
- []

Class should:
- method validation
- Check file existence
- file resolution (root + uri)
- directory handling
- index file
- Handle errors pages
- handle redirection
- Set headers
- Generate autoindex (if needed)
- content-type detection
- content-length header check
- keep-alive header? => won't handle

## Routing
### Router
- [] Match server
    - [] based on port
    - [] based on host header
    - [] fallback to default server

### LocationMatcher
- [] Match location
    - [] Longest prefix rule
    - [] exact match if implemented
    - [] return best match

## CGI
- [] timeout handling

## Edge cases to think of
- [] Prevent segmentation fault when handling a cUrl request

## To clarify
- [] A request to your server should never hang indefinitely.=> add timeout request/response/connection?

