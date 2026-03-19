# POST

## 1. check if requested method is allowed for current location

## 2. check if Content-Length < client_max_body_size from config
### ?? 2.1 check if body.size() = Content-Length

## 3. build response for CGI if it's cgi

## 4. build response for upload if it's upload
    UPLOAD if:
- method is POST
- is not cgi
- has upload_allowed
        upload_location in config

## 5. else error 403
