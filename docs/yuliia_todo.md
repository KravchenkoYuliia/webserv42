[+] accept only port
[+] accept localhost
[+] server{} is valid
[+] if no location in server -> after parsing check
    if location.empty()
        location is "/"

[+] if twice the same location -> error
[+] listen can be
    port
    12.0.0.1:port
    localhost:port

[] same pairs interface:port can only have once `default_server`
        80 default_server
        80 default_server --->>> error

        8080 default_server
        4242 default_server --->>> OK
    check after parsing



[] no Host in request -> server_name is useless - is ignored
    server 8080

    server 8080

    first server is considered as default except is there is `default_server` for other one

[] there is Host in request -> choose server accordingly to server_name

    has bigger priority than `default_server`

[] add TOKEN new_line to stop loop for cases like
    server_name bla bla1
    root www;

[] add a protection : multiple `listen` are impossible

[] create templates for repeated functions like
    parseServerNameInServer
    parseIndexInServer

## listen
    only server
## server_name
    only server
## root
    server
    location
## index
    server
    location
## error page
    server
    location
## autoindex
    server
    location
## client_max_body_size
server
location

