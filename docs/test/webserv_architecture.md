## 1. parse .config file 
    - get ports
        ports = [8080, 8081, 8082]

## 2. 
    epoll_fd = epoll_create() call

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







