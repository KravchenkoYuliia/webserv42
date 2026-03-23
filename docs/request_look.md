## Brave request
GET / HTTP/1.1\r\n
Host: localhost:8080\r\n
Connection: keep-alive
sec-ch-ua: "Not:A-Brand";v="99", "Brave";v="145", "Chromium";v="145"
sec-ch-ua-mobile: ?0
sec-ch-ua-platform: "Linux"
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8
Sec-GPC: 1
Accept-Language: en-US,en;q=0.7
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br, zstd\r\n
\r\n

GET /favicon.ico HTTP/1.1
Host: localhost:8080
Connection: keep-alive
sec-ch-ua-platform: "Linux"
User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/145.0.0.0 Safari/537.36
sec-ch-ua: "Not:A-Brand";v="99", "Brave";v="145", "Chromium";v="145"
sec-ch-ua-mobile: ?0
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8
Sec-GPC: 1
Accept-Language: en-US,en;q=0.7
Sec-Fetch-Site: same-origin
Sec-Fetch-Mode: no-cors
Sec-Fetch-Dest: image
Referer: http://localhost:8080/
Accept-Encoding: gzip, deflate, br, zstd

coding: gzip, deflate, br, zstd


## Firefox request
GET / HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate, br, zstd
DNT: 1
Sec-GPC: 1
Connection: keep-alive
Upgrade-Insecure-Requests: 1
Sec-Fetch-Dest: document
Sec-Fetch-Mode: navigate
Sec-Fetch-Site: none
Sec-Fetch-User: ?1
Priority: u=0, i


ConnectionHandler default constructor called
EPOLLIN case
GET /favicon.ico HTTP/1.1
Host: localhost:8080
User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:140.0) Gecko/20100101 Firefox/140.0
Accept: image/avif,image/webp,image/png,image/svg+xml,image/*;q=0.8,*/*;q=0.5
Accept-Language: fr,fr-FR;q=0.8,en-US;q=0.5,en;q=0.3
Accept-Encoding: gzip, deflate, br, zstd
DNT: 1
Sec-GPC: 1
Connection: keep-alive
Referer: http://localhost:8080/
Sec-Fetch-Dest: image
Sec-Fetch-Mode: no-cors
Sec-Fetch-Site: same-origin
Priority: u=6


----------------------

curl -X POST "https://example.com/upload" \
  -H "Content-Type: multipart/form-data; boundary=----MyBoundary123" \
  --data-binary $'------MyBoundary123\r
Content-Disposition: form-data; name="username"\r
\r
john_doe\r
------MyBoundary123\r
Content-Disposition: form-data; name="file"; filename="example.txt"\r
Content-Type: text/plain\r
\r
Hello from file contents.\r
------MyBoundary123--\r'










-------------------------------

i am working on webserv, can you show me the different possible form of a request that contains a body?

-------------------------------


### multipart/form-data: used for file uploads

POST /upload HTTP/1.1
Host: example.com
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABC123
Content-Length: ...

------WebKitFormBoundaryABC123
Content-Disposition: form-data; name="username"

john
------WebKitFormBoundaryABC123
Content-Disposition: form-data; name="file"; filename="test.txt"
Content-Type: text/plain

Hello file content
------WebKitFormBoundaryABC123--



🔹 Body split into parts using a boundary
🔹 Each part has its own headers
🔹 Can mix text + binary files


--------------------
### text/plain

Simple raw text body.

POST /log HTTP/1.1
Host: example.com
Content-Type: text/plain
Content-Length: 13

Hello, world!

🔹 No structure, just raw text

---------------------

### Chunked Transfer Encoding

When size is unknown ahead of time.

POST /stream HTTP/1.1
Host: example.com
Transfer-Encoding: chunked

7
Mozilla
9
Developer
7
Network
0

🔹 No Content-Length
🔹 Body sent in chunks:

chunk size (hex)

data

ends with 0



curl -v POST http://localhost:8080/ \
  -H "Transfer-Encoding: chunked" \
  --data-binary $'7\r\nMozilla\r\n9\r\nDeveloper\r\n7\r\nNetwork\r\n0\r\n\r\n'


--------------------------

## chunked transfer with extension value

printf "POST / HTTP/1.1\r\nHost: localhost\r\nTransfer-Encoding: chunked\r\n\r\n5;foo=bar\r\nHello\r\n0\r\n\r\n" | nc localhost 8080

------------------------


curl -v -X POST http://localhost:8080/log \
  -H "Content-Type: text/plain" \
  --data "Hello, world!"


## Empty body

curl -v -X POST http://localhost:8080/log \
  -H "Content-Type: text/plain" \
  -H "Content-Length: 0"

## Large Body
curl -X POST http://localhost:8080/log \
  -H "Content-Type: text/plain" \
  --data "$(python3 -c 'print("A"*50000)')"

## Mismatch Content-Length
printf "POST /log HTTP/1.1\r\nHost: localhost\r\nContent-Type: text/plain\r\nContent-Length: 5\r\n\r\nHelloWorld" | nc localhost 8080


## Normal upload (not chunked)
curl -v -F "file=@test.txt" http://localhost:8080/upload

POST /upload HTTP/1.1
Host: localhost
Content-Type: multipart/form-data; boundary=----123
Content-Length: 12345

------123
Content-Disposition: form-data; name="file"; filename="a.txt"

(file content here)
------123--



## chunked upload
curl -v -F "file=@test.txt" \
  -H "Transfer-Encoding: chunked" \
  http://localhost:8080/upload


POST /upload HTTP/1.1
Host: localhost
Transfer-Encoding: chunked
Content-Type: multipart/form-data; boundary=----123



## multipart

curl -v -X POST http://localhost:8080/upload \
  -H "Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABC123" \
  --data-binary $'------WebKitFormBoundaryABC123\r\nContent-Disposition: form-data; name="username"\r\n\r\njohn\r\n------WebKitFormBoundaryABC123\r\nContent-Disposition: form-data; name="file"; filename="test.txt"\r\nContent-Type: text/plain\r\n\r\nHello file content\r\n------WebKitFormBoundaryABC123--\r\n'
