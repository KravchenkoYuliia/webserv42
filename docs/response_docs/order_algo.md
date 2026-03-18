| Method |        what it does                                            |
|--------|----------------------------------------------------------------|
|   GET  |download smth from site(client) to server(our local directories)|
|  POST  |upload smth to site(client)                                     |
| DELETE |delete smth from server(our local directories)                  |

### 1. make a map from vector<ServerConfig>
	port -> vector<ServerConfig>
	port -> vector<ServerConfig>
	port -> vector<ServerConfig>

### 2. make a map during bind-listen-accept
	client_fd = port
	client_fd = port
	client_fd = port

### 3. parse the request
- method
- URI
- host
- version
- body (GET does not have a body)

### 4.find corresponding server in vector of servers

	8080 -> { [Server1], [Server2], [Server3] }

- check server_name
	if not match or no Host in request:
	- find default_server directive
		if no default_server directive:
		- take the first server with matching port

### 5. find corresponding location in current server

	GET /images/cat.png

	server {
		location /
		location /upload
		location /images !!! PREFIX MATCH -> choose this location
		...
	}

if no location match:
	use the data in server block

### 5.1 check return because it override all other directives
- location return has first priority
	return -->> direct return
- server return is checked if there is no return in location
	return -->> direct return

### 5.2 check allowed methods in location
	if request has method that is not allowed in this location -> error 405 Method Not Allowed

- GET -> body is ignored
- DELETE - > body is ignored
- POST - > check client_max_body_size and upload_allowed/upload_location

#### 5.2.1 POST.client_max_body_size
- check in location if ( client_max_body_size != kNotSpecified )
- otherwise check in server

if `Content-Length` > client_max_body_size ---> error 413 Payload Too Large
read body and count its length to double check if it's not larger than `client_max_body_size`

#### 5.2.2 POST.upload
if upload is allowed ---> server uploads file to upload_location

example:

request			`POST /upload/cat.png`
upload_location		`/var/www/uploads`

file is saved in `/var/www/uploads/cat.png`



### 5.3 define root directory
- location root has first priority
	root + URI in request

- if no root in location or no matching location at all -> take URI from request and add it to root written in server

example:
	root 			/html
	URI from request 	/images/cat.png

	------> /html/images/cat.png

### 5.4 check if CGI is allowed in this location
	CGI means that server does not give a file to client but execute directly a program

instead of:
```client -> server -> file.html -> client
```


it will be:
```client -> server -> run the script -> script generate HTML -> client
```
### 5.5 check if path (root + URI) is file or directory

- file : send this file
	code 200 OK

	if file does not exists -->> error 404 Not Found

- directory :
	check location index -> send if exists

		if not : check server index -> send if exists

		if not: check autoindex

### 5.6 check autoindex
priority first location autoindex
if not written -> server autoindex

- if `on`
	generate directory listing
- if `off`
	403 Forbidden

### 5.7 if there is an error - find corresponding error_page
- location error_page

- if none -> server error_page and return corresponding pagee
