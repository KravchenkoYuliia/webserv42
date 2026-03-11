[x] accept only port
[x] accept localhost
[x] server{} is valid
[x] if no location in server -> after parsing check
    if location.empty()
        location is "/"

[x] if twice the same location -> error
[x] listen can be
    port
    12.0.0.1:port
    localhost:port

[x] no Host in request -> server_name is useless - is ignored
    server 8080

    server 8080

    first server is considered as default except is there is `default_server` for other one

[x] there is Host in request -> choose server accordingly to server_name

    has bigger priority than `default_server`

[x] add TOKEN new_line to stop loop for cases like
    server_name bla bla1
    root www;

[x] protect cases like listen inside location block



[x] same pairs interface:port can only have once `default_server`
        80 default_server
        80 default_server --->>> error

        8080 default_server
        4242 default_server --->>> OK
    check after parsing

[x] add a protection : multiple `listen` are impossible

[x] set default error pages -> for now it's
    400
    403
    404
    405
    413
    500

[x] check that it only is 1 return in scope
[x] return not only code+page
    code 200 does not return page:
    return 200;
    return 200 'Ok';

[+] print location with operator<< (maybe server too)
[x] create a list of priority to response ( index in location > index of server/ return / path etc)
[x] protect client_max_body_size 50a;
[x] fix valgrind error
    values not initialized for server {
    client_nax_body_size 50;
    listen 123;
    location /123 {}
}

[x] fix default value of client_max_body_size in location
    must be NOT_SPECIFIED(-1) instead of 0
[x] check if empty file --> error
[x] protect invalid methods (accept only GET POST DELETE)

[x] fix : server_name is not possible in location -> must be error
    check with listen as well


[x] add upload_allowed
[x] add upload_location


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
## upload_allowed and upload_location
    location



## Server:
- listen
- server_name

## Location :
- allowed_methods
- upload_allowed
- upload_location

## Server and location (can be in both)  :
- client_max_body_size
- index
- root
- error_page
- autoindex
- return
