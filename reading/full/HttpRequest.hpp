#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <string>
#include <map>

class HttpRequest
{
public:
    enum Method { GET, POST, DELETE, UNKNOWN };

    HttpRequest()
        : method_(UNKNOWN)
        , content_length_(0)
        , is_chunked_(false)
        , is_multipart_(false)
    {}

    // ── Setters (used by RequestParser) ──────────────────────────────────────

    void setMethod(Method m)                                   { method_ = m; }
    void setUri(const std::string &uri)                        { uri_ = uri; }
    void setVersion(const std::string &v)                      { version_ = v; }
    void setHeader(const std::string &k, const std::string &v) { headers_[k] = v; }
    void appendBody(const std::string &data)                   { body_ += data; }
    void setContentLength(size_t n)                            { content_length_ = n; }
    void setChunked(bool v)                                    { is_chunked_ = v; }
    void setIsMultipart(bool v)                                  { is_multipart_ = v; }
    void setMultipartBoundary(const std::string &b)            { multipart_boundary_ = b; }

    // ── Getters (used by ResponseBuilder) ────────────────────────────────────

    Method             getMethod()            const { return method_; }
    const std::string& getUri()               const { return uri_; }
    const std::string& getVersion()           const { return version_; }
    const std::string& getBody()              const { return body_; }
    size_t             getContentLength()     const { return content_length_; }
    bool               isChunked()            const { return is_chunked_; }
    bool               isMultipart()          const { return is_multipart_; }
    const std::string& getMultipartBoundary() const { return multipart_boundary_; }

    const std::map<std::string, std::string>& getHeaders() const { return headers_; }

    // Case-insensitive header lookup — HTTP headers are case-insensitive (RFC 7230)
    std::string getHeader(const std::string &key) const
    {
        std::map<std::string, std::string>::const_iterator it = headers_.find(toLower(key));
        return it != headers_.end() ? it->second : "";
    }

    void reset()
    {
        method_            = UNKNOWN;
        uri_.clear();
        version_.clear();
        headers_.clear();
        body_.clear();
        content_length_    = 0;
        is_chunked_        = false;
        is_multipart_      = false;
        multipart_boundary_.clear();
    }

private:
    static std::string toLower(const std::string &s)
    {
        std::string out = s;
        for (size_t i = 0; i < out.size(); ++i)
            out[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(out[i])));
        return out;
    }

    Method                             method_;
    std::string                        uri_;
    std::string                        version_;
    std::map<std::string, std::string> headers_; // keys stored lowercase
    std::string                        body_;
    size_t                             content_length_;
    bool                               is_chunked_;
    bool                               is_multipart_;
    std::string                        multipart_boundary_;
};

#endif
