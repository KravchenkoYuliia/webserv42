_This project has been created as part of the 42 curriculum by jgossard, yukravch_  

## Description  
This project is about writting our own HTTP server.  
HTTP is one of the most widely used protocols on the internet.  
Functioning of the HTTP server:  
- must have a configuration file ".conf"  
- parse a request  
- create response  
- if any error occurs in request or during building the response - error response is created

## Instruction
Compile with `make`  
execute with `./webserv <configuration file>` ( the default configuration is provided in `config/default.conf` )

Send a request from:
- an actual browser (localhost or 127.0.0.1) ( welcome page of our server has button to run cgi scripts, upload a file and set a cookie)
- telnet
- curl

Request - basic examples:
- GET / HTTP/1.0
- GET /images/ HTTP/1.1
  host: webserv.com
- POST /upload/ HTTP/1.0
  Content-Type: text/plain
  Content-Length: 5

  body_
- DELETE <path to the file you want to delete>  HTTP/1.0

## Resources

### General
- [Stack Overflow](https://stackoverflow.com/questions)
- [Geeks for geeks](https://www.geeksforgeeks.org/)
- [cppreference.com](https://en.cppreference.com/)
- [cplusplus.com](https://cplusplus.com/reference/)

### Network Programming
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
- [OSI model](https://www.youtube.com/watch?v=OTwp3xtd4dg)
- [Unix Network Programming (TCP, Socket, Poll, etc.)](https://notes.shichao.io/unp/)
- [I/O Multiplexing: The select and poll Functions](https://notes.shichao.io/unp/ch6/)
- [Epoll syntax](https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/)
- [IPC - Inter Process Communication](https://www.geeksforgeeks.org/operating-systems/inter-process-communication-ipc/)

### Sockets
- [Socket in Computer Network](https://www.geeksforgeeks.org/computer-networks/socket-in-computer-network/)
- [Introduction to Sockets + Client Server Model with code example](https://www.labs.cs.uregina.ca/330/Sockets/sockets.html)
- [Socket Programming with TCP client-server diagram](https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html)
- [Socket Code Examples](https://web.stanford.edu/~ouster/cs190-winter23/sockets/)
- [Socket programming in C++](https://www.geeksforgeeks.org/cpp/socket-programming-in-cpp/)
- [Communication par sockets TCP/IP](https://du-isn.gricad-pages.univ-grenoble-alpes.fr/2-sr/Reseaux/3--cours_reseaux--sockets_java.pdf)
- [Socket in Computer Network](https://www.geeksforgeeks.org/computer-networks/socket-in-computer-network/)
- [Create TCP Echo Server using Libev ](https://codefundas.blogspot.com/2010/09/create-tcp-echo-server-using-libev.html)

### Ports
- [What is a computer port?](https://www.cloudflare.com/learning/network-layer/what-is-a-computer-port/)

### Client-Server Architecture
- [Client-Server Architecture Explained](https://blog.algomaster.io/p/client-server-architecture-explained)

### Nginx
- [Nginx Master-Worker Architecture: From Zero to Production](https://medium.com/%40nomannayeem/nginx-master-worker-architecture-from-zero-to-production-c451ee8e44ca)
- [How NGINX's Event-Driven Architecture Handles Million Concurrent Connections ? Overview of NGNIX's Event Driven Non-Blocking I/O Architecture](https://engineeringatscale.substack.com/p/nginx-millions-connections-event-driven-architecture)
- [Location block in config of NGINX](https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms)
- [Mime Types in Nginx](https://freedium-mirror.cfd/https://medium.com/@alendennis77/understanding-mime-types-and-their-usage-in-nginx-d0ed94bf7363)
- [Nginx.conf doc](https://docsaid.org/en/docs/nginx-notes/supplementary/nginx-conf-intro/)
- [Localhost doc](https://localhost.co/8080)
- [Indexer](https://blog.fernvenue.com/archives/nginx-file-indexer/)
- [How to Increase client_max_body_size](https://oneuptime.com/blog/post/2025-12-16-increase-client-max-body-size-nginx/view)

### HTTP
- [Creating an HTTP Server from Scratch](https://medium.com/@sakhawy/creating-an-http-server-from-scratch-ed41ef83314b)
- [HTTP Cats](https://http.cat/)
- [Webserv tutorials with good HTTP response flow-chart](https://m4nnb3ll.medium.com/webserv-building-a-non-blocking-web-server-in-c-98-a-42-project-04c7365e4ec7)
- [HTTP : HyperText Transfer Protocol - by MDN (Mozilla Developper Network)](https://developer.mozilla.org/fr/docs/Web/HTTP)

## epoll()
- [Mastering epoll()](https://medium.com/@m-ibrahim.research/mastering-epoll-the-engine-behind-high-performance-linux-networking-85a15e6bde90)
- [Epoll tuto](https://suchprogramming.com/epoll-in-3-easy-steps/)

### Reactor Pattern
- [Wikipedia - Reactor Pattern](https://en.wikipedia.org/wiki/Reactor_pattern)
- [Reactor - An Object Behavioral Pattern for Demultiplexing and Dispatching Handles for Synchronous Events](https://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf)
- [Acceptor - A Design Pattern for Passively Initializing Network Services](https://www.dre.vanderbilt.edu/~schmidt/PDF/Acceptor.pdf)
- [Reactor - Concurreny Pattern - Nantes University presentation](https://patterns.univ-nantes.io/reactor.html#/)
- [Pattern-Oriented Software Architecture: Patterns for Concurrent and Networked Objects](https://www.dre.vanderbilt.edu/~schmidt/POSA/POSA2/)

### RFC
- [HTTP](https://www.rfc-editor.org/rfc/rfc7230.html)
- [HTTP](https://datatracker.ietf.org/doc/html/rfc2616)
- [Cookie](https://datatracker.ietf.org/doc/html/rfc6265)
- [Syntax specifications](https://www.rfc-editor.org/rfc/rfc5234.html)
- [Cgi](https://datatracker.ietf.org/doc/html/rfc3875)
- []
