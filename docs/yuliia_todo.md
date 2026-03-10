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

[+] no Host in request -> server_name is useless - is ignored
    server 8080

    server 8080

    first server is considered as default except is there is `default_server` for other one

[+] there is Host in request -> choose server accordingly to server_name

    has bigger priority than `default_server`

[+] add TOKEN new_line to stop loop for cases like
    server_name bla bla1
    root www;

[+] protect cases like listen inside location block



[+] same pairs interface:port can only have once `default_server`
        80 default_server
        80 default_server --->>> error

        8080 default_server
        4242 default_server --->>> OK
    check after parsing

[+] add a protection : multiple `listen` are impossible

[+] set default error pages -> for now it's
    400
    403
    404
    405
    413
    500

[] check that it only is 1 return in scope
[] return not only code+page
    code 200 does not return page:
    return 200;
    return 200 'Ok';

[] print location with operator<< (maybe server too)

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

## allowed_methods
    location
## return
    location
    server


Pull request message
## Upgrade error checking for server directives :
- listen
- server_name

## Add location directive :
- allowed_methods

## Add server and location directives (can be in both)  :
- client_max_body_size
- index
- root
- error_page
- autoindex
- return
