#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <map>
#include <sstream>

class HttpResponse
{
public:
    HttpResponse()
        : status_code_(200)
        , status_text_("OK")
    {
        // sensible defaults
        headers_["Server"]     = "webserv/1.0";
        headers_["Connection"] = "keep-alive";
    }

    // ── Setters ───────────────────────────────────────────────────────────────

    void setStatus(int code, const std::string &text)
    {
        status_code_ = code;
        status_text_ = text;
    }

    void setHeader(const std::string &key, const std::string &value)
    {
        headers_[key] = value;
    }

    void setBody(const std::string &body)
    {
        body_ = body;
        // automatically update Content-Length whenever body is set
        std::ostringstream oss;
        oss << body_.size();
        headers_["Content-Length"] = oss.str();
    }

    void setClose()
    {
        headers_["Connection"] = "close";
    }

    // ── Getters ───────────────────────────────────────────────────────────────

    int                getStatusCode() const { return status_code_; }
    const std::string& getBody()       const { return body_; }

    // ── Serialization ─────────────────────────────────────────────────────────

    // Produces the full HTTP/1.1 response as a single string ready for send()
    // Format:
    //   HTTP/1.1 <code> <text>\r\n
    //   <header>: <value>\r\n
    //   ...
    //   \r\n
    //   <body>
    std::string serialize() const
    {
        std::ostringstream oss;

        oss << "HTTP/1.1 " << status_code_ << " " << status_text_ << "\r\n";

        for (std::map<std::string, std::string>::const_iterator it = headers_.begin();
             it != headers_.end(); ++it)
            oss << it->first << ": " << it->second << "\r\n";

        oss << "\r\n"; // blank line separating headers from body
        oss << body_;

        return oss.str();
    }

private:
    int                                status_code_;
    std::string                        status_text_;
    std::map<std::string, std::string> headers_;
    std::string                        body_;
};

#endif
