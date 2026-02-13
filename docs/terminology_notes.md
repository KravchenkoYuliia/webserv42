## `URL` = Uniform Resource Locators
	is the address that specifies where a resource is located on the internet and how to access it

http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
If the port is empty or not given, port 80 is assumed.

## Method `GET`
	retrieves a resource without modifying the server state
	[Give me the resource]

## Method `POST`
	sends data to the server to create or modify a resource
	[Do smth with this resource]

## Method `DELETE`
	requests the server to remove a specified resource
	[delete this resource]


## request-response

- google "flowers"
- browser=client ( example: Mozilla ) creates a HTTP request and sends it to Google=host=origin server
	example:
			GET /search?q=flowers HTTP/1.1
			Host: www.google.com  

	example:

			GET /hello.txt HTTP/1.1
			User-Agent: curl/7.64.1
			Host: www.example.com
			Accept-Language: en, mi



- host listens for request -> receives a request -> parses it -> executes internal logique( algorithms, data base, indices ) -> creates a HTTP response
	example:
			HTTP/1.1 200 OK
			Content-Type: text/html
			Content-Length: 12345

			<html> ... result ... </html>

	example:



### to see a real http request
		```bash
		python3 -m http.server 8080 &
		curl -v http://localhost:8080/
		```


