#include "../../include/server/TcpServer.hpp"

#include "../../include/http/HttpParser.hpp"
#include "../../include/http/HttpRequest.hpp"
#include "../../include/http/HttpResponse.hpp"
#include "../../include/http/Router.hpp"

#include <iostream>
#include <cstring>
#include <unordered_map>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/event.h>

static bool setNonBlocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags < 0)
    {
        return false;
    }

    if (fcntl(
            fd,
            F_SETFL,
            flags | O_NONBLOCK
        ) < 0)
    {
        return false;
    }

    return true;
}

TcpServer::TcpServer(int port)
    : server_fd(-1),
      port(port)
{
}

TcpServer::~TcpServer()
{
    if (server_fd >= 0)
    {
        close(server_fd);
    }
}

void TcpServer::start()
{
    // =========================
    // Create TCP socket
    // =========================

    server_fd = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (server_fd < 0)
    {
        std::cerr
            << "socket() failed\n";

        return;
    }

    // =========================
    // Allow address reuse
    // =========================

    int opt = 1;

    if (setsockopt(
            server_fd,
            SOL_SOCKET,
            SO_REUSEADDR,
            &opt,
            sizeof(opt)
        ) < 0)
    {
        std::cerr
            << "setsockopt() failed\n";

        close(server_fd);

        return;
    }

    // =========================
    // Configure server address
    // =========================

    sockaddr_in server_address;

    std::memset(
        &server_address,
        0,
        sizeof(server_address)
    );

    server_address.sin_family =
        AF_INET;

    server_address.sin_addr.s_addr =
        INADDR_ANY;

    server_address.sin_port =
        htons(port);

    // =========================
    // Bind socket
    // =========================

    if (bind(
            server_fd,
            reinterpret_cast<sockaddr*>(
                &server_address
            ),
            sizeof(server_address)
        ) < 0)
    {
        std::cerr
            << "bind() failed\n";

        close(server_fd);

        return;
    }

    // =========================
    // Start listening
    // =========================

    if (listen(
            server_fd,
            SOMAXCONN
        ) < 0)
    {
        std::cerr
            << "listen() failed\n";

        close(server_fd);

        return;
    }

    // =========================
    // Make server socket non-blocking
    // =========================

    if (!setNonBlocking(server_fd))
    {
        std::cerr
            << "Failed to set non-blocking mode\n";

        close(server_fd);

        return;
    }

    std::cout
        << "Server listening on port "
        << port
        << std::endl;

    // =========================
    // Create kqueue
    // =========================

    int kqueue_fd = kqueue();

    if (kqueue_fd < 0)
    {
        std::cerr
            << "kqueue() failed\n";

        close(server_fd);

        return;
    }

    // =========================
    // Register listening socket
    // =========================

    struct kevent change_event;

    EV_SET(
        &change_event,
        server_fd,
        EVFILT_READ,
        EV_ADD,
        0,
        0,
        nullptr
    );

    if (kevent(
            kqueue_fd,
            &change_event,
            1,
            nullptr,
            0,
            nullptr
        ) < 0)
    {
        std::cerr
            << "kevent registration failed\n";

        close(kqueue_fd);

        close(server_fd);

        return;
    }

    // =========================
    // Event loop
    // =========================

    const int MAX_EVENTS = 32;

    struct kevent events[MAX_EVENTS];

    // =========================
    // Per-client stream buffers
    // =========================

    std::unordered_map<
        int,
        std::string
    > client_buffers;

    while (true)
    {
        int event_count =
            kevent(
                kqueue_fd,
                nullptr,
                0,
                events,
                MAX_EVENTS,
                nullptr
            );

        if (event_count < 0)
        {
            std::cerr
                << "kevent() wait failed\n";

            continue;
        }

        // =========================
        // Process all ready events
        // =========================

        for (int i = 0;
             i < event_count;
             i++)
        {
            int event_fd =
                static_cast<int>(
                    events[i].ident
                );

            // =========================
            // New incoming connection
            // =========================

            if (event_fd == server_fd)
            {
                sockaddr_in client_address;

                socklen_t client_len =
                    sizeof(client_address);

                int client_fd =
                    accept(
                        server_fd,
                        reinterpret_cast<
                            sockaddr*
                        >(&client_address),
                        &client_len
                    );

                if (client_fd < 0)
                {
                    if (errno != EAGAIN &&
                        errno != EWOULDBLOCK)
                    {
                        std::cerr
                            << "accept() failed\n";
                    }

                    continue;
                }

                // =========================
                // Make client non-blocking
                // =========================

                if (!setNonBlocking(client_fd))
                {
                    close(client_fd);

                    continue;
                }

                std::cout
                    << "Client connected\n";

                // =========================
                // Register client socket
                // =========================

                struct kevent client_event;

                EV_SET(
                    &client_event,
                    client_fd,
                    EVFILT_READ,
                    EV_ADD,
                    0,
                    0,
                    nullptr
                );

                if (kevent(
                        kqueue_fd,
                        &client_event,
                        1,
                        nullptr,
                        0,
                        nullptr
                    ) < 0)
                {
                    std::cerr
                        << "Failed to register client socket\n";

                    close(client_fd);

                    continue;
                }
            }

            // =========================
            // Existing client sent data
            // =========================

            else
            {
                char buffer[4096];

                ssize_t bytes_received =
                    recv(
                        event_fd,
                        buffer,
                        sizeof(buffer),
                        0
                    );

                // =========================
                // Client disconnected
                // =========================

                if (bytes_received == 0)
                {
                    std::cout
                        << "Client disconnected\n";

                    client_buffers.erase(
                        event_fd
                    );

                    close(event_fd);

                    continue;
                }

                // =========================
                // recv() failure
                // =========================

                if (bytes_received < 0)
                {
                    if (errno != EAGAIN &&
                        errno != EWOULDBLOCK)
                    {
                        std::cerr
                            << "recv() failed\n";

                        client_buffers.erase(
                            event_fd
                        );

                        close(event_fd);
                    }

                    continue;
                }

                // =========================
                // Append bytes to stream buffer
                // =========================

                client_buffers[event_fd]
                    .append(
                        buffer,
                        bytes_received
                    );

                std::string& stream_buffer =
                    client_buffers[event_fd];

                // =========================
                // Extract complete requests
                // =========================

                while (true)
                {
                    size_t request_end =
                        stream_buffer.find(
                            "\r\n\r\n"
                        );

                    // Incomplete request
                    if (request_end ==
                        std::string::npos)
                    {
                        break;
                    }

                    // Include delimiter length
                    request_end += 4;

                    // Extract ONE request
                    std::string raw_request =
                        stream_buffer.substr(
                            0,
                            request_end
                        );

                    // Remove consumed bytes
                    stream_buffer.erase(
                        0,
                        request_end
                    );

                    // =========================
                    // Parse request
                    // =========================

                    HttpRequest request =
                        HttpParser::parse(
                            raw_request
                        );

                    std::cout
                        << request.method
                        << " "
                        << request.path
                        << std::endl;

                    // =========================
                    // Route request
                    // =========================

                    HttpResponse response =
                        Router::route(
                            request
                        );

                    response.setHeader(
                        "Connection",
                        "keep-alive"
                    );

                    std::string response_text =
                        response.toString();

                    // =========================
                    // Send response
                    // =========================

                    send(
                        event_fd,
                        response_text.c_str(),
                        response_text.size(),
                        0
                    );
                }
            }
        }
    }

    close(kqueue_fd);
}