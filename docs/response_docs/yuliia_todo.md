[x] fix config parser -> if redirection code does not have smth after -> error
[x] check permission of directory and file to open file
[x] return file
    ex: return 200 /file.html;
    if exists -> return
    if does not -> 404 not found
    if exist but no permission -> 403 forbidden
[x] delete default error_pages in case if no error_page directive in server

[x] return only works with html -> change to any type
[x] if no location match -> it's not an error
    server must work without location with server data
[x] listing must be a link to the correct files
[x] upgrade default html

[x] redirection makes second request to a new location
    add body to redirection

[x] return 200 /text.html
    returns plain text - only one element or config error
    no second request - just text

[x] add other lines from request example
[x] check build listing if uri does not have /
