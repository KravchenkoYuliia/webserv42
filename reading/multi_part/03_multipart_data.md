Great, you’re at the exact point where good class boundaries will save you a lot of pain later—especially with epoll ET.

I’ll give you a **clean, minimal, production-friendly design** for both `HttpRequest` and `MultipartParser`.

---

# 🧩 1. `HttpRequest` — what to add

You want `HttpRequest` to **own the parsed multipart result**, not the parser.

## ✅ New data structures

```cpp
struct UploadedFile {
    std::string filename;
    std::string contentType;
    std::string data;        // raw file content
};

struct MultipartData {
    std::map<std::string, std::string> fields;
    std::map<std::string, UploadedFile> files;
};
```

---

## ✅ Add to `HttpRequest` (private members)

```cpp
class HttpRequest {
private:
    MultipartData multipartData_;
    bool hasMultipart_;

public:
    HttpRequest() : hasMultipart_(false) {}
```

---

## ✅ Add public methods

```cpp
// Store parsed multipart result
void setMultipartData(const MultipartData& data)
{
    multipartData_ = data;
    hasMultipart_ = true;
}

// Check if request contains multipart data
bool hasMultipart() const
{
    return hasMultipart_;
}

// Access parsed multipart data
const MultipartData& getMultipartData() const
{
    return multipartData_;
}
```

---

## 🔍 Optional helpers (VERY useful)

These make your response builder cleaner:

```cpp
// Get a form field
std::string getField(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = multipartData_.fields.find(name);
    if (it != multipartData_.fields.end())
        return it->second;
    return "";
}

// Check if file exists
bool hasFile(const std::string& name) const
{
    return multipartData_.files.find(name) != multipartData_.files.end();
}

// Get uploaded file
const UploadedFile& getFile(const std::string& name) const
{
    return multipartData_.files.at(name);
}
```

---

# 🧩 2. `MultipartParser` — what it should look like

Keep it **stateless** for now (simpler, safer).

---

## ✅ Header

```cpp
class MultipartParser {
public:
    MultipartParser();
    ~MultipartParser();

    MultipartData parse(const std::string& body,
                        const std::string& contentType);

private:
    std::string extractBoundary(const std::string& contentType);

    void parsePart(const std::string& part, MultipartData& result);

    std::map<std::string, std::string> parseHeaders(const std::string& headersBlock);

    std::string trim(const std::string& str);
};
```

---

## ✅ Key method: `parse`

```cpp
MultipartData MultipartParser::parse(const std::string& body,
                                     const std::string& contentType)
{
    MultipartData result;

    std::string boundary = extractBoundary(contentType);
    if (boundary.empty())
        return result;

    std::string delimiter = "--" + boundary;
    std::string endDelimiter = delimiter + "--";

    size_t pos = 0;

    while (true)
    {
        size_t start = body.find(delimiter, pos);
        if (start == std::string::npos)
            break;

        start += delimiter.length();

        // End reached
        if (body.compare(start, 2, "--") == 0)
            break;

        // Skip CRLF
        if (body.compare(start, 2, "\r\n") == 0)
            start += 2;

        size_t end = body.find(delimiter, start);
        if (end == std::string::npos)
            break;

        std::string part = body.substr(start, end - start);
        parsePart(part, result);

        pos = end;
    }

    return result;
}
```

---

## ✅ Parse each part

```cpp
void MultipartParser::parsePart(const std::string& part,
                               MultipartData& result)
{
    size_t headerEnd = part.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return;

    std::string headersBlock = part.substr(0, headerEnd);
    std::string body = part.substr(headerEnd + 4);

    std::map<std::string, std::string> headers = parseHeaders(headersBlock);

    std::string disposition = headers["Content-Disposition"];

    // Extract name + filename
    std::string name;
    std::string filename;

    size_t namePos = disposition.find("name=\"");
    if (namePos != std::string::npos)
    {
        size_t start = namePos + 6;
        size_t end = disposition.find("\"", start);
        name = disposition.substr(start, end - start);
    }

    size_t filePos = disposition.find("filename=\"");
    if (filePos != std::string::npos)
    {
        size_t start = filePos + 10;
        size_t end = disposition.find("\"", start);
        filename = disposition.substr(start, end - start);
    }

    if (!filename.empty())
    {
        UploadedFile file;
        file.filename = filename;
        file.contentType = headers["Content-Type"];
        file.data = body;

        result.files[name] = file;
    }
    else
    {
        result.fields[name] = trim(body);
    }
}
```

---

## ✅ Extract boundary

```cpp
std::string MultipartParser::extractBoundary(const std::string& contentType)
{
    size_t pos = contentType.find("boundary=");
    if (pos == std::string::npos)
        return "";

    return contentType.substr(pos + 9);
}
```

---

## ✅ Parse headers

```cpp
std::map<std::string, std::string>
MultipartParser::parseHeaders(const std::string& headersBlock)
{
    std::map<std::string, std::string> headers;

    std::istringstream stream(headersBlock);
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);

        size_t sep = line.find(": ");
        if (sep != std::string::npos)
        {
            std::string key = line.substr(0, sep);
            std::string value = line.substr(sep + 2);
            headers[key] = value;
        }
    }

    return headers;
}
```

---

## ⚠️ Important limitations (for later)

This version:

* ✅ works for **fully received body**
* ❌ not streaming-safe yet
* ❌ not binary-safe trimming
* ❌ assumes clean CRLF formatting

That’s fine for now.

---

# 🔄 Final flow (clean)

```cpp
if (request_parser_.isComplete())
{
    HttpRequest& req = request_parser_.getRequest();

    if (isMultipart(req))
    {
        MultipartParser mp;
        MultipartData data = mp.parse(req.getBody(),
                                      req.getHeader("Content-Type"));

        req.setMultipartData(data);
    }

    buildResponse(req);
}
```

---

# 🚀 If you want next step

The **real challenge** (and what will break under load) is:

👉 handling multipart parsing **incrementally with epoll ET**

If you want, I can show you:

* a **state machine multipart parser**
* how to handle **boundary split across recv() calls**
* and how nginx actually approaches this problem

That’s where your project goes from “works” → “solid”
