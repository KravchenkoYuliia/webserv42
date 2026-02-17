# Webser Architecture Plan


Configuration files
    - default file
    - other files depending on arguments given
    - several servers?

Each files must at least have:
    - interfaces: port pairs + others?
    - default error pages
    - max allowed size for client request bodies
    - routes
        - List of accepted HTTP methods for the route: GET POST DELETE  (for information other method are: PUT UPDATE  => CRUD (Create Read Update Delete ))
        - HTTP redirection
        - Directory where the requested file should be locate:
            - root
            - URL
            - mkdir a directory?
        - Enabling or disabling directory listing.
        - Default file to serve when the requested resource is a directory.
        - Uploading files from the clients to the server is authorized, and storage location is provided.
    - CGI handling
    - timeout definition?


- how to specify that we are using HTTP protocol?
is it with the bind method through the struct sockaddr_in addr?
    cf. => addr.sin_family = AF_INET;

- Where to define HTTP request and response format?
see subject question => NGINX may be used to compare headers and answer behaviours (pay attention to differences between HTTP versions)

- Where to define HTTP response status code? (200, 400, 500, etc.)

## 1. parse .config file
    - get ports
        client_ports = [8080, 8081, 8082]

## 2.
    epoll_fd = epoll_create() call
    - defining the timeout with epoll?

## 3. socket loop

    one server from .config = one port = one socket

    ```pseudo code
    server_fd = socket();
    setsockopt(server_fd);
    fcntl(server_fd, O_NONBLOCK);

    bind(server_fd, port);
    listen(server_fd);

    epoll_ctl(epoll_fd, ADD, server_fd); //wait for new connections
    ```
    now socket can see new clients

## 4. event loop while true

- wait event

- if server_fd -->> accept
- if client_fd -->> recv/send

- close







