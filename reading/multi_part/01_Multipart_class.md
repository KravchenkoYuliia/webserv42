Great — this is the **next big step** after chunked: integrating a clean `multipart/form-data` parser **without breaking your current architecture**.

I’ll show you **where it belongs**, **how to design it**, and give you a **minimal implementation strategy that passes 42**.

---

# 🧠 1. Where multipart parsing should happen

👉 **NOT inside `RequestParser`**

You were right to question this earlier.

---

## 🔥 Correct architecture

```text
[ConnectionHandler]
        ↓
[RequestParser]
   - parse request line
   - parse headers
   - parse body (CL or chunked)
        ↓
HttpRequest (complete body as string)
        ↓
[MultipartParser]   ← HERE
        ↓
Parsed fields / files
        ↓
[ResponseBuilder / Upload handler]
```

---

## ❌ Why NOT in RequestParser?

Because `RequestParser` should only:

* understand HTTP framing
* NOT interpret body content

👉 separation of concerns:

* HTTP parsing ≠ application parsing

---

# 🧱 2. When to trigger multipart parsing

In your `ConnectionHandler`:

```cpp
if (request_parser_.isComplete())
{
    const HttpRequest& req = request_parser_.getRequest();

    if (isMultipart(req))
    {
        MultipartParser mp;
        mp.parse(req.getBody(), req.getHeader("Content-Type"));
    }

    // then build response
}
```

---

## Helper:

```cpp
bool isMultipart(const HttpRequest& req)
{
    return req.getHeader("Content-Type")
        .find("multipart/form-data") != std::string::npos;
}
```

---

# 🧩 3. What multipart looks like

Example body:

```http
------123
Content-Disposition: form-data; name="file"; filename="a.txt"
Content-Type: text/plain

Hello world
------123--
```

---

## Key concept: **boundary**

From header:

```http
Content-Type: multipart/form-data; boundary=----123
```

👉 Boundary = `"----123"`

Used to split parts.

---

# 🏗️ 4. Minimal MultipartParser design

## Class

```cpp
class MultipartParser {
public:
    void parse(const std::string& body, const std::string& contentType);

private:
    std::string extractBoundary(const std::string& contentType);
    void parseParts(const std::string& body, const std::string& boundary);
};
```

---

# 🔧 5. Step-by-step implementation

---

## 🔹 Step 1: Extract boundary

```cpp
std::string MultipartParser::extractBoundary(const std::string& contentType)
{
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return "";

    return "--" + contentType.substr(pos + 9);
}
```

👉 Important: prepend `"--"`

---

## 🔹 Step 2: Split body into parts

```cpp
void MultipartParser::parse(const std::string& body, const std::string& contentType)
{
    std::string boundary = extractBoundary(contentType);
    if (boundary.empty())
        return;

    parseParts(body, boundary);
}
```

---

## 🔹 Step 3: Iterate over parts

```cpp
void MultipartParser::parseParts(const std::string& body, const std::string& boundary)
{
    size_t pos = 0;

    while (true)
    {
        size_t start = body.find(boundary, pos);
        if (start == std::string::npos)
            break;

        start += boundary.length();

        // end boundary
        if (body.substr(start, 2) == "--")
            break;

        // skip CRLF
        if (body.substr(start, 2) == "\r\n")
            start += 2;

        size_t end = body.find(boundary, start);
        if (end == std::string::npos)
            break;

        std::string part = body.substr(start, end - start);

        parseSinglePart(part);

        pos = end;
    }
}
```

---

## 🔹 Step 4: Parse one part

Each part:

```http
Headers

Body
```

---

```cpp
void parseSinglePart(const std::string& part)
{
    size_t header_end = part.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return;

    std::string headers = part.substr(0, header_end);
    std::string body = part.substr(header_end + 4);

    // remove trailing CRLF
    if (body.size() >= 2 && body.substr(body.size() - 2) == "\r\n")
        body.erase(body.size() - 2);

    // parse headers
    parsePartHeaders(headers, body);
}
```

---

## 🔹 Step 5: Extract file info

From:

```http
Content-Disposition: form-data; name="file"; filename="a.txt"
```

---

```cpp
void parsePartHeaders(const std::string& headers, const std::string& body)
{
    if (headers.find("filename=") != std::string::npos)
    {
        std::string filename = extractFilename(headers);

        std::ofstream out(filename.c_str());
        out << body;
        out.close();
    }
}
```

---

## 🔹 Extract filename

```cpp
std::string extractFilename(const std::string& headers)
{
    size_t pos = headers.find("filename=\"");
    if (pos == std::string::npos)
        return "upload.bin";

    pos += 10;
    size_t end = headers.find("\"", pos);
    return headers.substr(pos, end - pos);
}
```
```c++
#include <string>
#include <fstream>

void savePartToFile(const std::string& headers, const std::string& body)
{
    std::string filename = "upload.bin";  // default filename

    // Try to extract filename from headers
    size_t pos = headers.find("filename=\"");
    if (pos != std::string::npos)
    {
        pos += 10;  // move past 'filename="'
        size_t end = headers.find("\"", pos);

        if (end != std::string::npos && end > pos)
        {
            filename = headers.substr(pos, end - pos);  // use extracted filename
        }
        // else leave filename as "upload.bin"
    }

    // Write body to the file
    std::ofstream out(filename.c_str());
    if (out)  // make sure file opened successfully
    {
        out << body;
        out.close();
    }
}
```

---

# 🧪 6. Test with curl

```bash
curl -v -F "file=@test.txt" http://localhost:8080/upload
```

---

# ⚠️ 7. Common mistakes (you MUST avoid)

## ❌ Parsing multipart before full body

→ breaks chunked

## ❌ Forgetting `--` in boundary

→ nothing matches

## ❌ Not removing trailing `\r\n`

→ corrupt files

## ❌ Mixing HTTP parsing + multipart

→ spaghetti code

---

# 💡 8. Minimal 42-compliant approach

You do NOT need:

* streaming parsing
* memory optimization
* full RFC compliance

👉 Just:

* split by boundary
* extract filename
* write file

---

# 🧠 Final mental model

```text
RequestParser → builds FULL body
            ↓
MultipartParser → splits into parts
            ↓
Upload handler → saves files
```

---

If you want next, I can:

* review your integration into `ConnectionHandler`
* or give you a **clean class diagram (Request → Multipart → CGI)** to avoid future refactors
