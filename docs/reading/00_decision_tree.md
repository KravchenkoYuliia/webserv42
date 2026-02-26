Perfect 😄
You’re now at the “architecture refinement” phase of Webserv.

I’ll give you the three things clearly and implementation-oriented:

1. ✅ A ready-to-implement decision tree
2. ✅ A flowchart-style execution order
3. ✅ A clean C++ pseudo-architecture

Everything aligned with NGINX-like behavior (mandatory Webserv subset, no regex).

---

# 1️⃣ Ready-To-Implement Decision Tree

You can literally translate this into code.

---

## 🔹 STEP 0 — TCP Accepted

You already know:

* Which socket accepted the connection
* Therefore which `IP:port`

---

## 🔹 STEP 1 — Parse HTTP Request

If parsing fails → return:

```
400 Bad Request
```

---

## 🔹 STEP 2 — Select Server Block

1. Get list of servers bound to that IP:port
2. Extract `Host` header
3. Remove port if present (`example.com:8080`)
4. Try exact match with `server_name`
5. If none matches → use default server

   * `default_server` if declared
   * otherwise first declared for that IP:port

---

## 🔹 STEP 3 — Select Location

1. If exact match (`location = /path`) → use it
2. Otherwise choose longest prefix match
3. If none → use `/`

---

## 🔹 STEP 4 — If Location Has Redirect

If config says:

```
return 301 /new;
```

Immediately return:

```
HTTP/1.1 301 Moved Permanently
Location: /new
```

STOP. No filesystem lookup.

---

## 🔹 STEP 5 — Check Method

If method not allowed:

Return:

```
405 Method Not Allowed
Allow: GET, POST
```

STOP.

---

## 🔹 STEP 6 — Check Body Size

If `Content-Length > client_max_body_size`

Return:

```
413 Request Entity Too Large
```

STOP.

---

## 🔹 STEP 7 — Build Filesystem Path

If using `root`:

```
full_path = root + request_uri
```

---

## 🔹 STEP 8 — If Path Does Not Exist

Return:

```
404 Not Found
```

(Then apply error_page logic if configured.)

---

## 🔹 STEP 9 — If Path Is a File

Serve file → 200 OK

---

## 🔹 STEP 10 — If Path Is a Directory

Apply this strict order:

### 1. If no trailing slash:

Redirect to add slash (301)

Example:

```
/docs → /docs/
```

---

### 2. Try index files (in order)

For:

```
index index.html index.htm;
```

Try:

```
/docs/index.html
/docs/index.htm
```

First found → serve it.

---

### 3. If no index found:

If `autoindex on` → generate directory listing

Else → return:

```
403 Forbidden
```

---

That’s the full decision tree.

---

# 2️⃣ Flowchart-Style Execution Order

Here’s the same logic visually:

```
Connection accepted
        ↓
Parse request
        ↓
Valid HTTP?
        ↓ no → 400
        ↓ yes
Select server (Host + default)
        ↓
Select location (exact > longest prefix)
        ↓
Has redirect?
        ↓ yes → 301/302
        ↓ no
Method allowed?
        ↓ no → 405
        ↓ yes
Body size OK?
        ↓ no → 413
        ↓ yes
Build filesystem path
        ↓
Exists?
        ↓ no → 404
        ↓ yes
Is file?
        ↓ yes → 200 serve file
        ↓ no (directory)
Trailing slash?
        ↓ no → 301 add slash
        ↓ yes
Index file exists?
        ↓ yes → serve index
        ↓ no
Autoindex on?
        ↓ yes → generate listing
        ↓ no → 403
```

That’s your runtime logic blueprint.

---

# 3️⃣ Clean C++ Pseudo-Architecture

Now the clean architecture part.

---

## 🔹 Core Classes

### Config Layer

```cpp
class Location {
public:
    std::string path;
    std::string root;
    std::set<std::string> allowed_methods;
    bool autoindex;
    std::vector<std::string> index_files;
    bool has_redirect;
    int redirect_code;
    std::string redirect_target;
};

class Server {
public:
    std::vector<Listen> listens;
    std::vector<std::string> server_names;
    std::string root;
    size_t client_max_body_size;
    std::vector<Location> locations;
    std::map<int, std::string> error_pages;
};
```

---

### Runtime Layer

```cpp
class HttpRequest {
public:
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;
};

class HttpResponse {
public:
    int status;
    std::map<std::string, std::string> headers;
    std::string body;
};
```

---

### Router Layer

```cpp
class Router {
public:
    Server* selectServer(SocketKey, const HttpRequest&);
    Location* selectLocation(Server*, const std::string& uri);
};
```

---

### Handler Layer

```cpp
class RequestHandler {
public:
    HttpResponse handle(HttpRequest& request);
};
```

Inside `handle()` you implement the decision tree.

---

# 🔥 Clean Separation of Responsibilities

| Component       | Responsibility           |
| --------------- | ------------------------ |
| ConfigParser    | Parse config file        |
| ServerManager   | Manage sockets           |
| Router          | Select server + location |
| RequestHandler  | Apply NGINX logic        |
| ResponseBuilder | Build HTTP response      |

Never mix them.

---

# 🧠 Golden Rule for Webserv

Your code should read like this:

```
response = router.selectServer()
          → router.selectLocation()
          → handler.applyRules()
          → handler.buildResponse()
```

If your logic feels chaotic, it means layers are mixed.

---

# 🎯 If You Implement Exactly This

You will:

* Pass mandatory
* Have clean code
* Avoid spaghetti logic
* Impress evaluators

---

If you want next, I can give you:

* A full example walk-through of one request going through all layers
* Or the most common evaluator traps at 42 for Webserv

You’re very close to mastering this project 🔥
