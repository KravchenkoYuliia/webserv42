


Good question. Let me trace through your current code to show exactly where the copies happen, then show you how to fix them.

## Where the copies happen today

```cpp
// COPY 1: parseParts — substr creates a full copy of the part
std::string part = body.substr(start, end - start);
parsePart(part); // passed by value → COPY 2

// Inside parsePart:
std::string raw_headers = part.substr(0, header_end_pos);      // COPY 3
std::string part_body = part.substr(header_end_pos + ...);     // COPY 4

// Inside parsePartHeaders → UploadedFile constructor:
UploadedFile file(filename, content_type, body); // COPY 5 — stores into data_
multipart_data_.addFile(name, file);             // COPY 6 — push_back copies the UploadedFile
```

For a 50MB file part, you could briefly have 3–4 full copies of that 50MB alive at the same time.

---

## The fix: pass by const reference, delay the only necessary copy

The **only** unavoidable copy is the final storage into `UploadedFile::data_`. Every intermediate step should just pass views (references) of the original `body` string without copying.

### `parseParts` — remove the `substr` copy of the part

```cpp
// BEFORE
void MultipartParser::parseParts(const std::string& body, const std::string& boundary)
{
    size_t pos = 0;
    while (pos < body.size())
    {
        // ...
        std::string part = body.substr(start, end - start); // full copy
        parsePart(part);
        pos = end;
    }
}

// AFTER — pass offsets into the original body instead
void MultipartParser::parseParts(const std::string& body, const std::string& boundary)
{
    size_t pos = 0;
    while (pos < body.size())
    {
        size_t start = body.find(boundary, pos);
        if (start == std::string::npos)
            break;
        start += boundary.size();
        if (body.size() >= start + Http::Formatting::BOUNDARY_DELIMITER_SIZE
                && body.substr(start, Http::Formatting::BOUNDARY_DELIMITER_SIZE) == Http::Formatting::BOUNDARY_SUFFIX)
            break;
        if (body.size() >= start + Http::Formatting::CRLF_SIZE
                && body.substr(start, Http::Formatting::CRLF_SIZE) == Http::Formatting::CRLF)
            start += Http::Formatting::CRLF_SIZE;
        size_t end = body.find(boundary, start);
        if (end == std::string::npos)
            break;

        // No copy — pass the original body + offsets
        parsePart(body, start, end);
        pos = end;
    }
}
```

### `parsePart` — work with offsets, no substr for the body

```cpp
// BEFORE
void MultipartParser::parsePart(const std::string& part)
{
    size_t header_end_pos = part.find(Http::Formatting::HEADER_END);
    if (header_end_pos == std::string::npos)
        return;
    std::string raw_headers = part.substr(0, header_end_pos);             // copy
    std::string part_body   = part.substr(header_end_pos + Http::Formatting::HEADER_END_SIZE); // copy

    size_t body_end_pos = ParserUtils::findCRLF(part_body);
    if (part_body.size() >= Http::Formatting::CRLF_SIZE && body_end_pos != std::string::npos)
        part_body.erase(body_end_pos, Http::Formatting::CRLF_SIZE);       // mutates a copy anyway
    parsePartHeaders(raw_headers, part_body);
}

// AFTER — only copy raw_headers (small), pass body by offsets
void MultipartParser::parsePart(const std::string& body, size_t start, size_t end)
{
    size_t header_end_pos = body.find(Http::Formatting::HEADER_END, start);
    if (header_end_pos == std::string::npos || header_end_pos >= end)
        return;

    // Headers are small (tens of bytes) — copying is fine
    std::string raw_headers = body.substr(start, header_end_pos - start);

    // Body: just track where it starts and ends, no copy yet
    size_t body_start = header_end_pos + Http::Formatting::HEADER_END_SIZE;
    size_t body_end   = end;

    // Trim trailing CRLF without copying
    if (body_end >= body_start + Http::Formatting::CRLF_SIZE
            && body.substr(body_end - Http::Formatting::CRLF_SIZE, Http::Formatting::CRLF_SIZE) == Http::Formatting::CRLF)
        body_end -= Http::Formatting::CRLF_SIZE;

    parsePartHeaders(body, raw_headers, body_start, body_end);
}
```

### `parsePartHeaders` — single copy at the final storage point

```cpp
// BEFORE
void MultipartParser::parsePartHeaders(const std::string& raw_headers, const std::string& body)
{
    const std::string name         = extractName(raw_headers);
    std::string       filename;
    std::string       content_type;

    if (raw_headers.find(Http::MultiPart::FILENAME) != std::string::npos)
    {
        filename     = extractFilename(raw_headers);
        content_type = extractContentType(raw_headers);
        UploadedFile file(filename, content_type, body); // copy of body here
        multipart_data_.addFile(name, file);             // copy of UploadedFile
    }
    else
        multipart_data_.addField(name, body);            // copy of body here
}

// AFTER — one deliberate copy into storage, addFile takes by ref
void MultipartParser::parsePartHeaders(
    const std::string& body,        // original full body
    const std::string& raw_headers,
    size_t body_start,
    size_t body_end
){
    const std::string name         = extractName(raw_headers);
    std::string       content_type;
    std::string       filename;

    if (raw_headers.find(Http::MultiPart::FILENAME) != std::string::npos)
    {
        filename     = extractFilename(raw_headers);
        content_type = extractContentType(raw_headers);

        // THE one unavoidable copy — from original body into storage
        UploadedFile file(filename, content_type, body.substr(body_start, body_end - body_start));
        multipart_data_.addFile(name, file);
    }
    else
    {
        // Same — one copy into the fields map
        multipart_data_.addField(name, body.substr(body_start, body_end - body_start));
    }
}
```

---

## Updated signatures in the header

```cpp
private:
    void    parseParts(const std::string& body, const std::string& boundary);

    // now takes original body + offsets instead of a pre-sliced string
    void    parsePart(const std::string& body, size_t start, size_t end);

    void    parsePartHeaders(
                const std::string& body,
                const std::string& raw_headers,
                size_t body_start,
                size_t body_end
            );
```

---

## Summary of what changed

| Step | Before | After |
|---|---|---|
| `parseParts` | `substr` copy of each part | passes offsets into original |
| `parsePart` | `substr` copy of headers + body | copies headers only (small), tracks body by offset |
| `parsePartHeaders` | receives full body copy | receives offsets, one `substr` at storage time |
| `addFile` | copies `UploadedFile` again | unchanged — still one copy into the map |

You go from **5–6 copies** of the file body down to **1**, which is the absolute minimum possible in C++98.
