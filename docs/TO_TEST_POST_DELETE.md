## telnet POST

POST /upload HTTP/1.1
Host: localhost:8080
Content-Type: multipart/form-data; boundary=----geckoformboundaryc32008197590b2edd3d6c8edc2b12404
Content-Length: 223

------geckoformboundaryc32008197590b2edd3d6c8edc2b12404
Content-Disposition: form-data; name="test"; filename="my_new_file"
Content-Type: text/x-csrc

hello

------geckoformboundaryc32008197590b2edd3d6c8edc2b12404--





## curl POST

echo "1234567890" > test.txt

curl -v -X POST http://localhost:8080/upload \
  -F "test=@test.txt"



## curl DELETE
curl -v -X DELETE http://localhost:8080/file.txt
