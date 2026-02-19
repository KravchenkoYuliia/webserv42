# Webser Architecture Plan

## 🕵️ Topics to research

### ❓ nginx config files
* Configuration files
    - default file
    - other files depending on arguments given
    - several servers?

* Each configuration files must at least have:
    - ✅  interfaces: port pairs + others? ➡️ use `listen 127.0.0.1:8080;`
    - ✅ default error pages ➡️ use `error_page 404 /404.html;`
    - ✅ max allowed size for client request bodies ➡️ use `client_max_body_size 10M;`
    - routes
        - ✅ List of accepted HTTP methods for the route: GET POST DELETE  (for information other method are: PUT PATCH  => CRUD Action (Create Read Update Delete )) ➡️ limit_except GET POST {
                deny all;
            }
            ➡️ others possibility : methods_allowed GET POST; (non nginx compliant)
        - ✅ HTTP redirection ➡️
        - Directory where the requested file should be locate:
            - ✅ root
            - ✅ URL ➡️ use location {}
            - mkdir a directory
        - ✅ Enabling or disabling directory listing.  ➡️ use `autoindex on;` / `autoindex off;`
        - ✅ Default file to serve when the requested resource is a directory. ➡️ this is controlled by the `index` directive
        - Uploading files from the clients to the server is authorized, and storage location is provided.
            - ➡️ nginx use some fsatcgi tools to allowed uploading file
            - ➡️ we can add some keywords like `upload_allowed on;` and `upload_storage_location /directory_path;`
    - CGI handling
    - ✅ timeout definition?  ➡️ use `keepalive_timeout 65;`...seems to be facultative

### ❓ HTTP Protocol
- how to specify that we are using HTTP protocol?
is it with the bind method through the struct sockaddr_in addr?
    cf. => addr.sin_family = AF_INET;

- Where to define HTTP request and response format?
see subject question => NGINX may be used to compare headers and answer behaviours (pay attention to differences between HTTP versions)

- Where to define HTTP response status code? (200, 400, 500, etc.)


##
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

---

## Config file Rules to respect

### Default server
###
### When requesting a directory:

NGINX does this in order:
1. Try index files
2. If none found:
    - If autoindex on → generate listing
    - If autoindex off → 403

---



