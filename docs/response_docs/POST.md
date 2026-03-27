# POST

## 1. check if requested method is allowed for current location

## 2. check if Content-Length < client_max_body_size from config
### ?? 2.1 check if body.size() = Content-Length

## 3. build response for CGI if it's cgi

## 4. build response for upload if it's upload

    UPLOAD if:
- method is POST
- is not cgi
- has upload_allowed - if no -> 403
        upload_location in config

if NO body in request --> 400
## 5. else error 403


## 6. Content-Type
#### 6.1 file with no extension with simple text inside/C/C++/Python/images

Content-Type: multipart/form-data;
Content-Disposition: form-data; name="test"; filename="text"

#### 6.2 script
Content-Type: multipart/form-data;

## 7 Success
201 Created
Location: path_to_created_file
