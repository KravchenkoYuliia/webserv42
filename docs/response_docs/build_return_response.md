# return

## create smth for code=meaning correlation

    switch {
        case 200 :
            return "OK";
        case 404 :
            return "Not Found";
    }
Success:
- 200
- 201 (POST upload) successfulyy created
- 204 success but no body (DELETE)

Redirection codes:
below

Client error:
- 400
- 403
- 404
- 405
- 413 (body is bigger than client_max_b_s)
- 414 ? (URI too long)

Server error:
- 500 (smth broken in server)
- 501 ( method not implemented by server )
- 502 (cgi error)
- 504 ( timeout cgi)

## if code is redirection
redirection:
- 301
- 302
- 303 ?
- 307 ?
- 308

if after code is smth (URL redirection):
    response's HEADER must have `Location:` directive + value from config (URL redirection)
else
    config error

NO body or create html page with explination that page moved to href url



## smth (text or url) after code
if there is smth after code ---> it's put to `body`
else
    just return code
    Content-Length: 0

TODO : fix config parser -> if redirection code does not have smth after -> error
TODO: check permission of directory and file to open file 
if smth after code starts with `/` --> it's path
    find - open - read - return text
