#pragma once

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"

class Router
{
public:
    static HttpResponse route(
        const HttpRequest& request
    );
};