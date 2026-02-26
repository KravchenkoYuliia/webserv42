# Q

i am working on webserv project from school 42

i am learning how nginx is working and started to wrote the following:
```
http {
	# to define type used by files
	# types {
        # text/html html;
        # text/css css;
    # }

	# or use directly the file from nginx
	include /etc/nginx/mime.types;

	# server context
	server {
		listen 8080; // listen to the port specified
		server_name test_nginx.test; // name use by the url; can be reached by using http://test_nginx.test


		# location context

		location /test # will match any URI starting with test => called a prefix match

		location = /test {
			return status_code "Message to return.\n";
		} # this syntax used a '=' sign and is called an exact match, will match only /test,any request else that /test will get a 404 response

		location ~ /test[0-9] {
			return 200 "Hello world.\n";
		} # regex match used a '~' sign and is called a regex match. it is by default case sensitive
		# To turn this into case insensitive, you'll have to add a * after the ~ sign.

		location ~* /test[0-9] {
			return 200 "Hello world 8.\n";
		}

		# a regex match has more priority than a prefix match

		# nginx come will a list of embedded variables than can be access by doing $variable_name
		# see full list => https://nginx.org/en/docs/varindex.html

		set $name $arg_name; # $arg_<query string name>

		return 200 "Name - $name\n";



		# redirect and rewrites example

		root /srv/test/static-demo; # used by default the file named static-demo from the folder ./srv/

        location = /index_page {
                return 307 /index.html;
        }

        location = /about_page {
                return 307 /about.html; # redirect http://url/abou-page will redirect to http://rul/about.html
        }
	}
}
```
is it correct? don't hesitate to update and add annotation or to replace the content with generic one, i will use that as a note on nginx config
Also do you know others usefull things that we can add inside  nginx config file?

------------------------

# A

Great initiative 👍 — understanding how **NGINX actually works internally** will help you a LOT for Webserv at 42.

Your config shows you already understand:

* contexts (`http`, `server`, `location`)
* prefix / exact / regex matches
* variables
* redirects

But there are **some syntax issues** and **some behavior misunderstandings**.
I'll first correct your config and turn it into a clean “learning note version”, then I’ll add important extra concepts you should know for Webserv.

---

# 🔧 Important Fixes

### ❌ Problems in your config

1. `// comments` → ❌ Not valid in nginx
   ✔ Use `#`

2. `location /test` → ❌ missing `{}`

3. You cannot put `return` directly inside `server` like that if you also use `location` blocks (it will override everything).

4. Regex priority explanation is incomplete (there is a specific matching order).

---

# ✅ Clean & Corrected Learning Version

Here is a **clean annotated version** you can use as notes:

```nginx
http {

    # Load MIME types (maps extensions to Content-Type header)
    include /etc/nginx/mime.types;

    # Default type if extension not found
    default_type application/octet-stream;

    server {
        listen 8080;
        server_name test_nginx.test;

        # Root directory for this server
        root /srv/test/static-demo;

        ############################################
        # LOCATION MATCHING EXAMPLES
        ############################################

        # 1️⃣ Exact match (highest priority)
        location = /test {
            return 200 "Exact match: /test\n";
        }

        # 2️⃣ Prefix match
        location /test {
            return 200 "Prefix match: /test...\n";
        }

        # 3️⃣ Regex match (case-sensitive)
        location ~ ^/test[0-9]$ {
            return 200 "Regex match (case sensitive)\n";
        }

        # 4️⃣ Regex match (case-insensitive)
        location ~* ^/hello[0-9]$ {
            return 200 "Regex match (case insensitive)\n";
        }

        ############################################
        # QUERY STRING VARIABLES
        ############################################

        # Example: http://localhost:8080/?name=42
        location /get_name {
            set $name $arg_name;
            return 200 "Name = $name\n";
        }

        ############################################
        # REDIRECTION EXAMPLES
        ############################################

        # Temporary redirect (307 keeps method)
        location = /index_page {
            return 307 /index.html;
        }

        location = /about_page {
            return 307 /about.html;
        }

        ############################################
        # STATIC FILE SERVING
        ############################################

        location / {
            try_files $uri $uri/ =404;
        }
    }
}
```

---

# 📌 VERY IMPORTANT — Location Matching Order

NGINX does NOT simply say "regex > prefix".

Actual order:

1. `=` exact match
2. Longest prefix match
3. `^~` prefix (stops regex search)
4. Regex `~` / `~*` (first match wins)
5. If no regex matched → use longest prefix

This is extremely important for Webserv.

---

# 🔥 Very Useful Directives You Should Know (For Webserv)

Here are things often required in 42 Webserv:

---

## 1️⃣ index directive

```nginx
index index.html index.htm;
```

If user requests `/`, nginx looks for those files.

---

## 2️⃣ autoindex (directory listing)

```nginx
location /files/ {
    autoindex on;
}
```

If no index file exists, nginx shows directory content.

You WILL implement this in Webserv.

---

## 3️⃣ client_max_body_size

```nginx
client_max_body_size 10M;
```

Limits upload size.

If exceeded → 413 error.

Very important for POST handling.

---

## 4️⃣ error_page

```nginx
error_page 404 /404.html;

location = /404.html {
    internal;
}
```

Used for custom error pages.

You MUST implement this in Webserv.

---

## 5️⃣ allowed methods restriction

```nginx
location /upload {
    limit_except GET POST {
        deny all;
    }
}
```

In Webserv you'll need to restrict allowed HTTP methods.

---

## 6️⃣ CGI handling (VERY IMPORTANT FOR 42)

Example:

```nginx
location ~ \.php$ {
    root /var/www/html;
    fastcgi_pass 127.0.0.1:9000;
    include fastcgi_params;
}
```

For Webserv, you won’t use FastCGI but you’ll implement your own CGI execution logic.

Important concepts:

* PATH_INFO
* SCRIPT_FILENAME
* environment variables
* fork + execve
* pipe communication

---

## 7️⃣ try_files (VERY IMPORTANT)

```nginx
location / {
    try_files $uri $uri/ /index.html;
}
```

Order:

* check file
* check directory
* fallback

You will need this logic in Webserv routing.

---

## 8️⃣ return vs rewrite

Simple redirect:

```nginx
return 301 https://example.com;
```

Rewrite URI internally:

```nginx
rewrite ^/old$ /new break;
```

---

# 🧠 Architecture Concept (Important for 42)

NGINX is:

* event-driven
* non-blocking
* uses epoll (Linux)
* master process
* worker processes

Master:

* reads config
* manages workers

Workers:

* handle connections

For Webserv, you're building a simplified event-driven server (select/poll/epoll/kqueue).

---

# ⚠️ Small Correction About Your Comment

You wrote:

> a regex match has more priority than a prefix match

❌ Not always true.

It depends on:

* whether `^~` is used
* longest prefix
* first matching regex

The order rule above is the correct mental model.

---

# 🎯 If Your Goal Is Webserv Mastery

You should deeply understand:

* HTTP request parsing
* Chunked transfer encoding
* Content-Length handling
* Persistent connections (keep-alive)
* CGI environment
* File descriptors management
* Non-blocking I/O
* select() / poll() / epoll()

If you want, I can next give you:

* 🔍 A simplified diagram of how nginx processes a request
* OR a "mapping between nginx config and what you must implement in Webserv"

Just tell me which one 😄

-----
# Q
yes to both please

also can you add in the clean annotated version, teh new concept that you have listed?

Perfect 😄 this is exactly the right mindset for Webserv.

I’ll give you:

1. ✅ A **complete clean annotated nginx config note** (with all the new useful concepts added)
2. 🧠 A **diagram of how nginx processes a request**
3. 🔁 A **mapping: nginx concepts → what YOU must implement in Webserv**

---

# ✅ COMPLETE Annotated NGINX Config (Learning Version)

You can use this as your structured note.

```nginx
############################################################
# HTTP CONTEXT
# Top-level configuration for HTTP servers
############################################################

http {

    ########################################################
    # MIME TYPES
    ########################################################

    # Maps file extensions to Content-Type header
    include /etc/nginx/mime.types;

    # Default if extension not found
    default_type application/octet-stream;

    ########################################################
    # PERFORMANCE / CONNECTION SETTINGS
    ########################################################

    sendfile on;                # zero-copy file sending
    keepalive_timeout 65;       # persistent connections timeout
    client_max_body_size 10M;   # limit upload size

    ########################################################
    # SERVER CONTEXT (Virtual Host)
    ########################################################

    server {
        listen 8080;
        server_name test_nginx.test;

        # Root directory for static files
        root /srv/test/static-demo;

        # Default index files
        index index.html index.htm;

        ####################################################
        # ERROR PAGES
        ####################################################

        error_page 404 /404.html;
        error_page 500 502 503 504 /50x.html;

        location = /404.html {
            internal;   # cannot be accessed directly
        }

        ####################################################
        # LOCATION MATCHING EXAMPLES
        ####################################################

        # 1️⃣ EXACT MATCH (highest priority)
        location = /test {
            return 200 "Exact match\n";
        }

        # 2️⃣ PREFIX MATCH
        location /test {
            return 200 "Prefix match\n";
        }

        # 3️⃣ PREFIX THAT STOPS REGEX CHECK
        location ^~ /images/ {
            root /data;
        }

        # 4️⃣ REGEX (case-sensitive)
        location ~ ^/user[0-9]+$ {
            return 200 "Regex match\n";
        }

        # 5️⃣ REGEX (case-insensitive)
        location ~* \.(jpg|jpeg|png)$ {
            root /var/www/images;
        }

        ####################################################
        # QUERY STRING VARIABLES
        ####################################################

        # Example: /hello?name=42
        location /hello {
            set $name $arg_name;
            return 200 "Hello $name\n";
        }

        ####################################################
        # STATIC FILE HANDLING
        ####################################################

        location / {
            # Order:
            # 1. Try exact file
            # 2. Try directory
            # 3. Return 404
            try_files $uri $uri/ =404;
        }

        ####################################################
        # AUTO INDEX (Directory Listing)
        ####################################################

        location /files/ {
            autoindex on;
        }

        ####################################################
        # METHOD RESTRICTION
        ####################################################

        location /upload {
            limit_except GET POST {
                deny all;
            }
        }

        ####################################################
        # REDIRECTION
        ####################################################

        location = /old_page {
            return 301 /new_page;
        }

        ####################################################
        # CGI (concept example)
        ####################################################

        location ~ \.php$ {
            # In real nginx:
            # fastcgi_pass 127.0.0.1:9000;
            # include fastcgi_params;

            # In Webserv:
            # you will fork + execve CGI binary
        }
    }
}
```

---

# 🧠 How NGINX Processes a Request (Step-by-Step)

Here is the simplified flow:

```
Client connects
      ↓
Master process accepts config
      ↓
Worker process handles connection
      ↓
Parse HTTP request
      ↓
Select correct server block (by port + server_name)
      ↓
Find best matching location
      ↓
Apply directives inside location
      ↓
Generate response (static file / redirect / CGI / error)
      ↓
Send response
      ↓
Keep-alive or close
```

Now more detailed internal flow:

```
Socket ready (epoll)
↓
read()
↓
Parse request line
↓
Parse headers
↓
Check:
    - method allowed?
    - body size allowed?
↓
If body:
    - read body
↓
Routing phase:
    - match server
    - match location
↓
Content phase:
    - static file?
    - autoindex?
    - CGI?
    - return?
↓
Response built
↓
write()
↓
Connection reused or closed
```

---

# 🔁 Mapping: NGINX → Webserv Implementation

This is the MOST important part for 42.

| NGINX Concept          | What You Must Implement in Webserv      |
| ---------------------- | --------------------------------------- |
| `listen`               | create socket + bind + listen           |
| `server_name`          | choose server based on Host header      |
| `location` matching    | build your own URI matching logic       |
| `root`                 | map URI → filesystem path               |
| `index`                | if URI is directory → search index file |
| `autoindex on`         | generate HTML listing                   |
| `error_page`           | custom error response                   |
| `client_max_body_size` | reject large POST with 413              |
| `limit_except`         | reject methods with 405                 |
| `return 301`           | build redirect response                 |
| `try_files`            | attempt multiple path resolutions       |
| CGI                    | fork + execve + pipes + env vars        |
| keepalive_timeout      | manage persistent connections           |
| worker process         | event loop with poll/select/epoll       |

---

# 🔥 The Most Difficult Parts in Webserv

From experience, students struggle most with:

### 1️⃣ Non-blocking I/O

You must:

* set sockets O_NONBLOCK
* handle partial reads
* handle partial writes

---

### 2️⃣ Chunked Transfer Encoding

You must:

* detect `Transfer-Encoding: chunked`
* parse chunk size (hex)
* assemble body

---

### 3️⃣ CGI

You must:

* fork()
* create pipes
* dup2()
* execve()
* set environment variables
* handle timeout
* read CGI output properly

---

### 4️⃣ Location Matching Logic

You must replicate nginx order:

1. exact match
2. longest prefix
3. regex
4. fallback

Most students get this wrong.

---

# 🏗️ Webserv Internal Architecture Suggestion

Good structure:

```
Server
 ├── SocketManager
 ├── EventLoop (poll/epoll)
 ├── ClientConnection
 │      ├── Request
 │      ├── Response
 │
 ├── Router
 ├── CGIHandler
 ├── ConfigParser
```

---

# 🎯 If You Want To Go Deeper

Next I can explain:

* 🔬 How to implement location matching algorithm step-by-step
* 🧪 How to test your server like an nginx tester
* ⚙️ How to design your config parser cleanly
* 📦 How to structure your HTTP parser
* 🔄 How to implement keep-alive correctly

Tell me which one you want next — and I’ll go deep like a 42 correction 😄
