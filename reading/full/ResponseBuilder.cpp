#include "ResponseBuilder.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>   // remove()
#include <cerrno>
#include <cstring>  // strerror

// ── build ─────────────────────────────────────────────────────────────────────
//
// Entry point. Routes to the correct handler based on HTTP method.
// Returns a 405 Method Not Allowed for anything other than GET/POST/DELETE.

HttpResponse ResponseBuilder::build(const HttpRequest &request)
{
    switch (request.getMethod())
    {
        case HttpRequest::GET:    return handleGet(request);
        case HttpRequest::POST:   return handlePost(request);
        case HttpRequest::DELETE: return handleDelete(request);
        default:                  return buildError(405, "Method Not Allowed");
    }
}

// ── handleGet ─────────────────────────────────────────────────────────────────
//
// Resolves URI → filesystem path → reads file → returns 200 with body.
//
// Flow:
//   URI "/index.html"
//     → resolvePath("/index.html") → "./www/index.html"
//     → open file
//     → 404 if not found
//     → 403 if not readable
//     → read entire file into string
//     → set Content-Type from extension
//     → return 200

HttpResponse ResponseBuilder::handleGet(const HttpRequest &request)
{
    std::string path = resolvePath(request.getUri());

    if (path.empty())
        return buildError(403, "Forbidden"); // ".." traversal attempt

    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file.is_open())
    {
        if (errno == EACCES)
            return buildError(403, "Forbidden");
        return buildError(404, "Not Found");
    }

    // read entire file into string
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string body = oss.str();

    HttpResponse response;
    response.setStatus(200, "OK");
    response.setHeader("Content-Type", getMimeType(path));
    response.setBody(body); // also sets Content-Length
    return response;
}

// ── handlePost ────────────────────────────────────────────────────────────────
//
// Writes the request body to a file under root_dir_.
// URI "/upload/file.txt" → writes to "./www/upload/file.txt"
//
// Returns 201 Created on success.
// Returns 500 if the file cannot be opened for writing.
//
// Note: does not create intermediate directories — that is out of scope
//       for a basic webserv implementation.

HttpResponse ResponseBuilder::handlePost(const HttpRequest &request)
{
    std::string path = resolvePath(request.getUri());

    if (path.empty())
        return buildError(403, "Forbidden");

    std::ofstream file(path.c_str(), std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return buildError(500, "Internal Server Error");

    file << request.getBody();
    if (file.fail())
        return buildError(500, "Internal Server Error");

    HttpResponse response;
    response.setStatus(201, "Created");
    response.setHeader("Content-Type", "text/plain");
    response.setBody("Created");
    return response;
}

// ── handleDelete ──────────────────────────────────────────────────────────────
//
// Removes the file at the resolved path using std::remove().
// Returns 200 OK on success.
// Returns 404 if the file does not exist.
// Returns 403 if removal was denied by the OS.

HttpResponse ResponseBuilder::handleDelete(const HttpRequest &request)
{
    std::string path = resolvePath(request.getUri());

    if (path.empty())
        return buildError(403, "Forbidden");

    if (std::remove(path.c_str()) != 0)
    {
        if (errno == ENOENT)
            return buildError(404, "Not Found");
        return buildError(403, "Forbidden");
    }

    HttpResponse response;
    response.setStatus(200, "OK");
    response.setHeader("Content-Type", "text/plain");
    response.setBody("Deleted");
    return response;
}

// ── resolvePath ───────────────────────────────────────────────────────────────
//
// Maps a URI to a real filesystem path under root_dir_.
// "/" is resolved to "/index.html" automatically.
// Any URI containing ".." returns "" (caller treats as 403).
//
// This is a basic implementation — does NOT handle query strings or fragments.
// Those should be stripped before this point in a full implementation.

std::string ResponseBuilder::resolvePath(const std::string &uri) const
{
    // block directory traversal
    if (uri.find("..") != std::string::npos)
        return "";

    std::string path = root_dir_;

    if (uri == "/")
        path += "/index.html";
    else
        path += uri;

    return path;
}

// ── getMimeType ───────────────────────────────────────────────────────────────
//
// Returns the MIME type string for a given file path based on its extension.
// Falls back to "application/octet-stream" for unknown types.

std::string ResponseBuilder::getMimeType(const std::string &path) const
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot);

    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css")                   return "text/css";
    if (ext == ".js")                    return "application/javascript";
    if (ext == ".json")                  return "application/json";
    if (ext == ".txt")                   return "text/plain";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".png")                   return "image/png";
    if (ext == ".gif")                   return "image/gif";
    if (ext == ".ico")                   return "image/x-icon";
    if (ext == ".pdf")                   return "application/pdf";

    return "application/octet-stream";
}

// ── buildError ────────────────────────────────────────────────────────────────
//
// Builds a minimal HTML error page.
// Example for 404:
//   <html><body><h1>404 Not Found</h1></body></html>

HttpResponse ResponseBuilder::buildError(int code, const std::string &text)
{
    std::ostringstream body;
    body << "<html><body><h1>" << code << " " << text << "</h1></body></html>";

    HttpResponse response;
    response.setStatus(code, text);
    response.setHeader("Content-Type", "text/html");
    response.setBody(body.str());
    return response;
}
