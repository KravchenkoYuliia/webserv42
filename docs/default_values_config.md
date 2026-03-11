# Default Configuration Values

## listen
(only in server)
If no `listen` directive is specified, the server defaults to port **8080**.
                                                        interface **0.0.0.0**

## location

If no `location` directive is specified, the server defaults to the path **/**

## server_name
(only in server)
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
If no `root` directive is specified, the server defaults to the root specified by us **html** in server
stays empty in location


## index
If no `index` directive is specified, the server defaults to the **index.html** in server
stays empty in location

## error_page
If no `error_page` directive is specified, the server defaults to his own map of error pages in server
stays empty in location

## autoindex
by default false AUTOINDEX_OFF in server if not written at all

in location : by default AUTOINDEX_NOT_SPECIFIED (because it can't be off by default) -> if no `autoindex` directive is sprecified in location -> look for `autoindex` directive in server

## client_max_body_size
by default 1m in server
and  kNotSpecified in location

## allowed_methods
(only in location)
if there is no directive `allowed_methods` all methods are autorized -->
GET POST DELETE

## return
stays empty

## upload_allowed + upload_location
(only in location)
if no upload_allowed -> it's `off` by default
upload location stays empty
