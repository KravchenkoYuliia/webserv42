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
[] listing must be a link to the correct files
[x] upgrade default html
[] return just code must add a corresponding html page to body

