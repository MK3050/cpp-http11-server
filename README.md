# HTTP Server in C++

A lightweight HTTP/1.1 server built from scratch in modern C++ using low-level BSD sockets and an event-driven architecture with `kqueue`.

This project was created to understand how web servers work internally — including TCP sockets, HTTP parsing, concurrency handling, non-blocking I/O, and event loops.

---

## Features

* HTTP/1.1 server implementation
* Built using BSD sockets in C++
* Event-driven architecture using `kqueue`
* Non-blocking socket handling
* Handles multiple simultaneous client connections
* Basic HTTP request parsing
* Sends valid HTTP responses
* Persistent server loop
* Clean modular project structure
* Minimal external dependencies

---

## Tech Stack

* C++17
* BSD Sockets
* kqueue (macOS / BSD)
* Makefile

---

## Project Structure

```bash
.
├── include/
│   └── TcpServer.hpp
├── src/
│   ├── main.cpp
│   └── TcpServer.cpp
├── build/
├── Makefile
├── README.md
└── .gitignore
```

---

## How It Works

1. The server creates a TCP socket.
2. It binds to a port and starts listening for incoming connections.
3. `kqueue` monitors socket events efficiently.
4. New client connections are accepted asynchronously.
5. Incoming HTTP requests are read and parsed.
6. The server sends an HTTP response back to the client.
7. Connections are cleaned up properly.

---

## Build Instructions

### Clone the Repository

```bash
git clone <your-repo-url>
cd <repo-name>
```

### Build

```bash
make
```

### Run

```bash
./server
```

---

## Testing the Server

Open a browser and visit:

```text
http://localhost:8080
```

Or test using curl:

```bash
curl http://localhost:8080
```

---

## Example HTTP Response

```http
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 13

Hello, World!
```

---

## Concepts Practiced

* TCP/IP networking
* Socket programming
* Event-driven systems
* Non-blocking I/O
* HTTP protocol basics
* System-level programming
* Concurrent connection handling
* OS networking APIs

---

## Future Improvements

* Full HTTP request parser
* Static file serving
* Thread pool support
* Configuration file support
* Logging system
* MIME type handling
* Better error handling
* Support for POST requests
* Keep-alive connections
* Reverse proxy features
* HTTPS/TLS support
* Linux `epoll` support

---

## License

This project is licensed under the MIT License.
