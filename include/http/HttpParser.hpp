#pragma once

#include "HttpRequest.hpp"

class HttpParser
{
public:
    static HttpRequest parse(const std::string& raw_request);
};