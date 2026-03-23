Perfect! Then let’s design `UploadedFile` and `MultipartData` as **fully encapsulated classes** with **getters and setters**, keeping everything clean and C++98-compatible. This approach respects your project’s style while integrating neatly with `HttpRequest`.

---

### 1️⃣ `UploadedFile` class

```cpp
// UploadedFile.h
#ifndef UPLOADEDFILE_H
#define UPLOADEDFILE_H

#include <string>

class UploadedFile {
private:
    std::string filename_;
    std::string contentType_;
    std::string data_;

public:
    UploadedFile() {}
    UploadedFile(const std::string& filename,
                 const std::string& contentType,
                 const std::string& data)
        : filename_(filename), contentType_(contentType), data_(data) {}

    // Getters
    const std::string& getFilename() const { return filename_; }
    const std::string& getContentType() const { return contentType_; }
    const std::string& getData() const { return data_; }

    // Setters
    void setFilename(const std::string& filename) { filename_ = filename; }
    void setContentType(const std::string& contentType) { contentType_ = contentType; }
    void setData(const std::string& data) { data_ = data; }
};

#endif
```

---

### 2️⃣ `MultipartData` class

```cpp
// MultipartData.h
#ifndef MULTIPARTDATA_H
#define MULTIPARTDATA_H

#include "UploadedFile.h"
#include <string>
#include <map>

class MultipartData {
private:
    std::map<std::string, std::string> fields_;
    std::map<std::string, UploadedFile> files_;

public:
    MultipartData() {}

    // -----------------------------
    // Field access
    // -----------------------------
    void addField(const std::string& name, const std::string& value) {
        fields_[name] = value;
    }

    bool hasField(const std::string& name) const {
        return fields_.find(name) != fields_.end();
    }

    const std::string& getField(const std::string& name) const {
        std::map<std::string, std::string>::const_iterator it = fields_.find(name);
        if (it != fields_.end()) return it->second;
        static std::string empty = "";
        return empty;
    }

    const std::map<std::string, std::string>& getFields() const { return fields_; }

    // -----------------------------
    // File access
    // -----------------------------
    void addFile(const std::string& name, const UploadedFile& file) {
        files_[name] = file;
    }

    bool hasFile(const std::string& name) const {
        return files_.find(name) != files_.end();
    }

    const UploadedFile& getFile(const std::string& name) const {
        return files_.at(name);
    }

    const std::map<std::string, UploadedFile>& getFiles() const { return files_; }
};

#endif
```

---

### 3️⃣ Updating `HttpRequest` to use these classes

```cpp
class HttpRequest {
private:
    std::string body_;
    std::map<std::string, std::string> headers_;
    MultipartData multipartData_;
    bool hasMultipart_;

public:
    HttpRequest() : hasMultipart_(false) {}

    // Body
    const std::string& getBody() const { return body_; }
    void setBody(const std::string& body) { body_ = body; }

    // Headers
    const std::string& getHeader(const std::string& key) const {
        std::map<std::string, std::string>::const_iterator it = headers_.find(key);
        if (it != headers_.end()) return it->second;
        static std::string empty = "";
        return empty;
    }
    void setHeader(const std::string& key, const std::string& value) { headers_[key] = value; }

    // Multipart data
    void setMultipartData(const MultipartData& data) {
        multipartData_ = data;
        hasMultipart_ = true;
    }

    bool hasMultipart() const { return hasMultipart_; }
    const MultipartData& getMultipartData() const { return multipartData_; }
};
```

---

✅ **Benefits of this approach**

* Fully encapsulated classes with getters/setters → matches your existing style.
* `HttpRequest` just stores a `MultipartData` object → clean separation of concerns.
* Easy to extend in the future (e.g., add file size, checksum, or streaming support).
* Works seamlessly with your `RequestParser` and `ConnectionHandler`.

---

If you want, I can also **rewrite your `MultipartParser` code to directly fill these `UploadedFile` and `MultipartData` objects**, so the integration into your existing `handleRead()` is ready to go. This would make it plug-and-play with your current design.

Do you want me to do that?
