# Question

for webserv can you list all the rules that need to be respected to mimic nginx behaviour?like for example the order checking when a directory is requesting

# Answer

Excellent. This is exactly the right question for Webserv.

You don’t need to re-implement *all* of NGINX — only the subset required by the subject — but you **must respect its behavioral logic order**.

Below is a clean, implementation-oriented checklist of rules to mimic NGINX behavior correctly for Webserv (mandatory part, no regex required).

---

# 🔥 1️⃣ High-Level Request Processing Order

For each HTTP request, your server should behave roughly like this:

```
1. Accept TCP connection
2. Parse HTTP request
3. Identify socket (IP:port)
4. Select correct server block
5. Select correct location block
6. Apply method restrictions
7. Apply body size limits
8. Handle redirection (if configured)
9. Build filesystem path
10. If path is directory → apply directory rules
11. If file exists → serve it
12. Otherwise → return correct error
```

Order matters.

---

# 🔥 2️⃣ Server Selection Rules (Virtual Hosting)

After connection is accepted:

### Step 1 — Match IP:Port

Only servers configured with the socket’s `listen` pair are candidates.

### Step 2 — Match `Host` header

* Extract host from:

  ```
  Host: example.com
  ```
* Ignore port part if present (`example.com:8080` → `example.com`)

### Step 3 — Selection logic

1. If `server_name` matches exactly → use that server
2. If none match → use default server for that IP:port
3. Default server =

   * the one marked `default_server`
   * otherwise the first declared for that listen pair

---

# 🔥 3️⃣ Location Matching Rules (Mandatory Webserv Subset)

Since regex is not required, implement:

## Priority Order:

### 1️⃣ Exact match

```
location = /test
```

Highest priority.

If request path == `/test` → stop searching.

---

### 2️⃣ Longest Prefix Match

Among all prefix locations:

```
location /images/
location /images/icons/
location /
```

Choose the one with the **longest matching prefix**.

Example:

Request:

```
/images/icons/logo.png
```

Best match:

```
/images/icons/
```

NOT the first declared.
NOT the shortest.
Longest prefix wins.

---

# 🔥 4️⃣ Redirection Rule

If location has:

```
return 301 /new_page;
```

You MUST:

* Immediately send redirect
* Skip filesystem lookup
* Skip index logic

Redirection overrides everything else.

---

# 🔥 5️⃣ Method Restriction Rules

If location restricts methods:

1. If method not allowed → return:

   ```
   405 Method Not Allowed
   ```
2. Include `Allow:` header listing permitted methods.

NGINX returns 405, not 403, for method mismatch.

---

# 🔥 6️⃣ Body Size Limit Rules

If:

```
client_max_body_size 10M;
```

Then:

* If `Content-Length` exceeds limit → return:

  ```
  413 Request Entity Too Large
  ```
* Must check BEFORE reading full body.

---

# 🔥 7️⃣ Filesystem Path Resolution Rules

If using `root`:

```
filesystem_path = root + full_request_URI
```

Example:

```
root /var/www;
Request: /images/pic.jpg
Result: /var/www/images/pic.jpg
```

If location overrides root, use location root.

---

# 🔥 8️⃣ When Requested Path Is a Directory

This is VERY important.

If resolved filesystem path is a directory:

## Apply rules in this exact order:

### Step 1 — Check for trailing slash

If request is:

```
/docs
```

But `/docs` is a directory, NGINX redirects to:

```
/docs/
```

(301 redirect)

You should mimic this.

---

### Step 2 — Try index files (in order)

If config says:

```
index index.html index.htm;
```

Check:

```
/docs/index.html
/docs/index.htm
```

First one found → serve it.

---

### Step 3 — If no index found

* If `autoindex on` → generate directory listing
* If `autoindex off` → return 403

---

# 🔥 9️⃣ File Serving Rules

If resolved path is a file:

* If exists → return 200
* If not exists → return 404

---

# 🔥 10️⃣ Error Page Rules

If:

```
error_page 404 /404.html;
```

Then:

1. Detect error (404)
2. Internally redirect to `/404.html`
3. Serve that file
4. Keep original status code (404)

Important: it’s an internal redirect.

---

# 🔥 11️⃣ Upload Handling Rules (Webserv Specific)

If method is POST:

1. Check size limit
2. If multipart:

   * Parse boundary
   * Extract file
   * Save to disk
3. Return appropriate status

NGINX normally passes to backend, but Webserv must do it manually.

---

# 🔥 12️⃣ Status Code Priority Order

If multiple problems exist, order matters.

Example:

* Method not allowed
* File not found

You must check:

1. Method validity
2. Body size
3. Redirection
4. Then filesystem

Not random order.

---

# 🎯 Complete Directory Handling Example

Request:

```
GET /docs HTTP/1.1
Host: example.com
```

Filesystem:

```
/var/www/docs/
    file1.txt
```

Config:

```
index index.html;
autoindex off;
```

Correct behavior:

1. `/docs` is directory
2. No trailing slash
3. Redirect → `/docs/`
4. Client requests `/docs/`
5. No index.html
6. autoindex off
7. Return 403

That’s real NGINX behavior.

---

# 🧠 Common 42 Mistakes

* Choosing first prefix match instead of longest
* Ignoring trailing slash redirect
* Returning 404 instead of 403 for directory without index
* Checking file existence before method validation
* Not stripping port from Host header
* Creating multiple sockets for same listen pair

---

# 🏁 Minimal Correct Webserv Behavior Checklist

You MUST implement:

✅ Server selection by Host
✅ Default server fallback
✅ Exact location match
✅ Longest prefix match
✅ Redirection
✅ Method restriction (405)
✅ client_max_body_size (413)
✅ Directory handling order
✅ Index resolution order
✅ autoindex behavior
✅ Error page internal redirect

---

If you want, I can next give you:

* A **ready-to-implement decision tree**
* Or a **flowchart-style execution order**
* Or a **clean C++ pseudo-architecture for request processing**

You’re now asking exactly the right questions to pass Webserv cleanly 🔥
