# Curl

## **. Testing with `curl`**

`curl` can automate tests for different headers and methods.

### **GET request**

```bash
curl -v http://localhost:8080/
```

* `-v` shows request and response headers.

### **POST request with Content-Length**

```bash
curl -v -X POST http://localhost:8080/ -d "Hello World"
```

* `-d` sets `Content-Length` automatically.

### **POST with chunked transfer**

```bash
curl -v -X POST http://localhost:8080/ -H "Transfer-Encoding: chunked" --data-binary @file.txt
```

### **Invalid cases**

* Missing `Host` header (should return false/400):

```bash
curl -v -X GET http://localhost:8080/ -H "Host:"
```

* Both `Content-Length` and `Transfer-Encoding` (should fail):

```bash
curl -v -X POST http://localhost:8080/ \
    -H "Content-Length: 10" \
    -H "Transfer-Encoding: chunked" \
    --data-binary "HelloWorld"
```

> Your parser should return `false` in these invalid cases.

---

If you want, I can create a **small table of `curl` commands vs expected parser behavior**, so you can test **all branches** of `validateHeaderSet()` systematically. This is super helpful for HTTP servers.

Do you want me to do that?
