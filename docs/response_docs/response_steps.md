# RESPONSE:
```
HTTP/1.0 200 OK\r\n
Content-Type: text/html\r\n
Content-Length:108\r\n
\r\n
<!DOCTYPE html>\n
<html>\n
<body>\n
<h1>Welcome to Webserv</h1>\n
<p>Team T-shirt rose.</p>\n
</body>\n
</html>"
```

### 1. version 1.0
can accept request 1.0 and 1.1
other version -> 505 HTTP Version Not Supported in RequestParser - checked in RequestParser
in any case build a response 1.0

### 2. status code

### 3. message after status

### 4. Content-type

can be:

- text/html
- text/css
- application/javascript
- image/png
- image/jpeg

if type is unknown
- application/octet-stream

### 5. Content-Length

length of BODY not of all the response



## Input data needed to create a response

### 1. data from request:
{
- port
- method (GET/POST/DELETE)
- uri  ( match to location)
- headers (Host name)
- body  (only with POST)
- -content-length ( to check if it's not bigger than client_max_body_size )

}

### 2. data from config
{
- port
- interface
- default_server
- server_name
- root
- index
- error_page
- autoindex
- client_max_body_size
- return_code
- location path
- allowed_methods
- upload_allowed
- upload_location
}

## Choose correct server with this 2 data structure and fill HttpResponse with correct data

## Data inside HttpResponse

{

- whole_path ( root + uri )
- index ( to return if directory )
- error_page ( to return if error )
- autoindex ( to know if to do listing or return error )
- //return_code ( ?? if return is in config file -> direct return so no need to store it ?? )
- allowed_methods ( to check if request's method is allowed )
- client_max_body_size ( to check if request's content-length is not bigger than this)
- upload_allowed ( for POST only )
- upload_location ( for POST only is upload is allowed )
- has_return ( bool )
- return_status ( code )
- return_string (redirection path )
- cgi_extension
- cgi_path

}
