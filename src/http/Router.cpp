#include "../../include/http/Router.hpp"

HttpResponse Router::route(
    const HttpRequest& request
)
{
    HttpResponse response;

    // =========================
    // Route: /
    // =========================

    if (request.path == "/")
    {
        response.setStatus(200, "OK");

        response.setHeader(
            "Content-Type",
            "text/plain"
        );

        response.setBody(
            "Welcome to the C++ HTTP server!\n"
        );
    }

    // =========================
    // Route: /hello
    // =========================

    else if (request.path == "/hello")
    {
        response.setStatus(200, "OK");

        response.setHeader(
            "Content-Type",
            "text/plain"
        );

        response.setBody(
            "Hello from /hello route!\n"
        );
    }

    // =========================
    // Route: /health
    // =========================

    else if (request.path == "/health")
    {
        response.setStatus(200, "OK");

        response.setHeader(
            "Content-Type",
            "application/json"
        );

        response.setBody(
            "{ \"status\": \"healthy\" }\n"
        );
    }

    // =========================
    // Route not found
    // =========================

    else
    {
        response.setStatus(404, "Not Found");

        response.setHeader(
            "Content-Type",
            "text/plain"
        );

        response.setBody(
            "404 Not Found\n"
        );
    }

    // Close connection after response
    response.setHeader(
        "Connection",
        "close"
    );

    return response;
}