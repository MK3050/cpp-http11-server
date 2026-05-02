#pragma once

#include <string>
#include <unordered_map>

class HttpResponse
{
private:
    int status_code;
    std::string status_message;

    std::unordered_map<std::string, std::string> headers;

    std::string body;

public:
    HttpResponse();

    void setStatus(int code,
                   const std::string& message);

    void setHeader(const std::string& key,
                   const std::string& value);

    void setBody(const std::string& body_content);

    std::string toString() const;
};