# Default Configuration Values

## listen

If no `listen` directive is specified, the server defaults to port **8080**.
                                                        interface **0.0.0.0**

## location

If no `location` directive is specified, the server defaults to the path **/**

## server_name

If no 'server_name' directive is specified, the std::vector server_name remains
empty.
During execution of the request:
    - find the corresponding ip:port. if there are multiple server with
    corresponding ports:

        - if server_name is not empty -> check host name. If there are multiple servers
                with the same server name or no Host in request:
                    - find default_server. If no default_server:
                            - take first server

## root
If no `root` directive is specified, the server defaults to the root specified by us **html**

## index
If no `index` directive is specified, the server defaults to the **index.html**
