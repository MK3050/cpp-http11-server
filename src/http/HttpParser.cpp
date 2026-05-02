#include "../../include/http/HttpParser.hpp"

#include <sstream>
#include <iostream>

HttpRequest HttpParser::parse(const std::string& raw_request)
{
    HttpRequest request;

    std::istringstream stream(raw_request);

    std::string line;

    // =========================
    // Parse request line
    // =========================

    if (std::getline(stream, line))
    {
        // Remove trailing '\r'
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        std::istringstream request_line(line);

        request_line >> request.method
                     >> request.path
                     >> request.version;
    }

    // =========================
    // Parse headers
    // =========================

    while (std::getline(stream, line))
    {
        // Header section ends at empty line
        if (line == "\r" || line.empty())
        {
            break;
        }

        // Remove trailing '\r'
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        // Find ':' separator
        size_t colon_pos = line.find(':');

        if (colon_pos == std::string::npos)
        {
            continue;
        }

        std::string key =
            line.substr(0, colon_pos);

        std::string value =
            line.substr(colon_pos + 1);

        // Remove leading space in value
        if (!value.empty() && value[0] == ' ')
        {
            value.erase(0, 1);
        }

        request.headers[key] = value;
    }

    // =========================
    // Parse body
    // =========================

    std::string body;
    std::string temp;

    while (std::getline(stream, temp))
    {
        body += temp;
    }

    request.body = body;

    return request;
}