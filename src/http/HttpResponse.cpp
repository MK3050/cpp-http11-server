#include "../../include/http/HttpResponse.hpp"

#include <sstream>

HttpResponse::HttpResponse()
    : status_code(200),
      status_message("OK")
{
}

void HttpResponse::setStatus(int code,
                             const std::string& message)
{
    status_code = code;
    status_message = message;
}

void HttpResponse::setHeader(const std::string& key,
                             const std::string& value)
{
    headers[key] = value;
}

void HttpResponse::setBody(const std::string& body_content)
{
    body = body_content;

    // Automatically update Content-Length
    headers["Content-Length"] =
        std::to_string(body.size());
}

std::string HttpResponse::toString() const
{
    std::ostringstream response;

    // =========================
    // Status line
    // =========================

    response << "HTTP/1.1 "
             << status_code
             << " "
             << status_message
             << "\r\n";

    // =========================
    // Headers
    // =========================

    for (const auto& header : headers)
    {
        response << header.first
                 << ": "
                 << header.second
                 << "\r\n";
    }

    // Empty line separates headers/body
    response << "\r\n";

    // =========================
    // Body
    // =========================

    response << body;

    return response.str();
}