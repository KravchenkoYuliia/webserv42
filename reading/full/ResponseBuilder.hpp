#ifndef RESPONSEBUILDER_HPP
#define RESPONSEBUILDER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include <string>

class ResponseBuilder
{
public:
    explicit ResponseBuilder(const std::string &root_dir)
        : root_dir_(root_dir)
    {}

    // Entry point: routes to the correct handler based on HTTP method
    HttpResponse build(const HttpRequest &request);

private:
    // ── Method handlers ───────────────────────────────────────────────────────

    // GET: resolve URI to file path, read it, return 200 with body
    //      → 404 if file not found, 403 if not readable
    HttpResponse handleGet(const HttpRequest &request);

    // POST: write request body to a file under root_dir_
    //       → 201 Created on success, 500 on write failure
    HttpResponse handlePost(const HttpRequest &request);

    // DELETE: remove the file at the resolved path
    //         → 200 OK on success, 404 if not found
    HttpResponse handleDelete(const HttpRequest &request);

    // ── Helpers ───────────────────────────────────────────────────────────────

    // Turns a URI like "/index.html" into root_dir_ + "/index.html"
    // Also resolves "/" to root_dir_ + "/index.html" (default index)
    // Rejects paths containing ".." to prevent directory traversal
    std::string  resolvePath(const std::string &uri) const;

    // Returns MIME type string from file extension
    // e.g. ".html" → "text/html", ".jpg" → "image/jpeg"
    std::string  getMimeType(const std::string &path) const;

    // Builds a minimal HTML error page and sets status code + body
    HttpResponse buildError(int code, const std::string &text);

    std::string root_dir_; // e.g. "./www" — base directory for static files
};

#endif
