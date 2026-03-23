Here are three minimal `curl` examples you can use to test different POST request types:

---

## 1) `text/plain`

Send a raw plain-text body:

```bash
curl -X POST https://example.com/endpoint \
  -H "Content-Type: text/plain" \
  --data "Hello, this is plain text"
```

---

## 2) `Transfer-Encoding: chunked`

`curl` automatically uses chunked transfer when the body size isn’t known in advance (e.g., piping input). Example:

```bash
echo "This is sent chunked" | curl -X POST https://example.com/endpoint \
  -H "Transfer-Encoding: chunked" \
  -H "Content-Type: text/plain" \
  --data-binary @-
```

Or using a file stream:

```bash
curl -X POST https://example.com/endpoint \
  -H "Transfer-Encoding: chunked" \
  -H "Content-Type: text/plain" \
  --data-binary @largefile.txt
```

---

## 3) `multipart/form-data`

Send form fields and files:

```bash
curl -X POST https://example.com/endpoint \
  -F "field1=value1" \
  -F "field2=value2"
```

With a file upload:

```bash
curl -X POST https://example.com/endpoint \
  -F "file=@test.txt" \
  -F "description=Sample upload"
```

---

## Tips

* Use `-v` for verbose output (debug headers, chunking behavior, etc.).
* Use `--trace-ascii debug.txt` if you want to inspect raw request details.
* You don’t need to manually set `Content-Type` for multipart; `curl` handles it.

---

If you want, I can tailor these to a specific backend (like Express, FastAPI, or Spring) so you can verify how each is parsed.
