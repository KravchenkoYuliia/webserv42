# GET
return what is asked in URI
Construct Root + uri and use `stat` to check if this path exists
if not ---> 404

S_ISREG -> is file
S_ISDIR -> is directory
else -> 403 (symbolic link etc are not implemented)

## uri is file
check permissions
error: 403

make function that handle the file according to extension of the file
( can be anything )
if no extension :
`Content-Type: application/octet-stream`

good -> 200

## uri is directory ( has / at the end )
find the file from config `index`

directory + index:
if exists -> 200 read the file and put it to body
if exists but no permissions (for directory or file) -> 403 error
if doesn't exist:
    try next index in vector till finding an existing index
    if nothing exists:
### check autoindex
if on --> generate body `html` with list of files in this directory
        if opendir fail = no permission for this directory --> 403
if off --> 403 ( listing is forbidden )



## if file is image --> read in binary
std::ifstream file(path, std::ios::binary);

accepted extension: .jpeg .png
    else: 415


std::ifstream file(path, std::ios::binary);
std::string content(
    (std::istreambuf_iterator<char>(file)),
    std::istreambuf_iterator<char>()
);

Listing:
name    last_modified   size

