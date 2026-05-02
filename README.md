# HTTP/1.1 Web Server in C++

> High-performance, event-driven HTTP server built from scratch using raw BSD sockets and kqueue — no external networking libraries.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![Platform](https://img.shields.io/badge/platform-macOS%20%2F%20BSD-orange)
![License](https://img.shields.io/badge/license-MIT-green)

---

## Overview

This project implements the core architecture found in real-world web servers by combining:

- Non-blocking sockets
- Kernel event notification
- Persistent connections
- Manual HTTP parsing
- TCP stream reassembly
- Event-driven I/O

No external networking libraries are used — only raw BSD sockets, POSIX system calls, and the C++17 standard library.

---

## Architecture

The server follows an event-driven reactor pattern:

```
Client
   ↓
TCP Socket
   ↓
kqueue Event Loop
   ↓
Non-blocking recv()
   ↓
Per-client stream buffer
   ↓
HTTP Parser
   ↓
Router
   ↓
HTTP Response Builder
   ↓
send()
```

---

## Features

- Raw TCP socket server
- HTTP/1.1 request parsing
- HTTP response generation
- Path-based routing
- Persistent connections (Keep-Alive)
- Non-blocking sockets
- Event-driven architecture using kqueue
- TCP stream reassembly
- Per-client connection buffers
- Multiple requests per connection
- Partial read handling
- Kernel-level I/O multiplexing

---

## Why kqueue?

Instead of using one thread per connection, the server uses macOS/BSD's native event notification system: `kqueue()`.

This allows:

- Scalable concurrent connections
- Low CPU usage
- Efficient socket readiness notification
- Single-threaded event-driven I/O

This is conceptually similar to Linux `epoll`, Nginx event loops, and Redis networking architecture.

---

## Key Networking Concepts

### Non-Blocking Sockets

Sockets are configured using:

```c
fcntl(fd, F_SETFL, O_NONBLOCK)
```

This prevents the server from stalling on slow clients.

### TCP Stream Reassembly

TCP is a byte-stream protocol and does not preserve request boundaries. The server maintains:

- Persistent per-client buffers
- Incremental request reconstruction
- Fragmented request handling

This allows correct handling of partial reads, coalesced requests, and keep-alive connections.

### Persistent Connections

HTTP/1.1 connections remain open by default. The server supports:

- Multiple requests per TCP connection
- Connection reuse
- Keep-alive behavior

---

## Routes

| Route | Method | Description |
|-------|--------|-------------|
| `/` | GET | Welcome message |
| `/hello` | GET | Simple text response |
| `/health` | GET | JSON health check |
| `*` | ANY | 404 Not Found |

---

## Project Structure

```
http-server/
├── Makefile
├── README.md
├── main.cpp
│
├── include/
│   ├── server/
│   │   └── TcpServer.hpp
│   │
│   └── http/
│       ├── HttpRequest.hpp
│       ├── HttpParser.hpp
│       ├── HttpResponse.hpp
│       └── Router.hpp
│
├── src/
│   ├── server/
│   │   └── TcpServer.cpp
│   │
│   └── http/
│       ├── HttpParser.cpp
│       ├── HttpResponse.cpp
│       └── Router.cpp
│
└── .gitignore
```

---

## Build & Run

### Requirements

- macOS or BSD-based system
- C++17 compiler (`clang++` or `g++`)
- `make`

### Compile

```bash
make
```

### Run

```bash
./server
```

Expected output:

```
Server listening on port 8080
```

---

## Testing

### Browser

Open:

```
http://localhost:8080
```

### curl

```bash
curl http://localhost:8080/hello
curl http://localhost:8080/health
```

### netcat (raw HTTP)

```bash
nc localhost 8080
```

Then type:

```
GET /hello HTTP/1.1
Host: localhost

```

### Example Response

```
HTTP/1.1 200 OK
Content-Type: text/plain
Content-Length: 27
Connection: keep-alive

Hello from /hello route!
```

---

## Technologies

| Technology | Role |
|------------|------|
| C++17 | Language |
| BSD sockets | Raw TCP networking |
| kqueue | Kernel I/O multiplexing |
| POSIX APIs | System-level calls |
| HTTP/1.1 | Application protocol |

---

## Future Improvements

- [ ] Static file serving
- [ ] MIME type detection
- [ ] HTTP POST body parsing
- [ ] Chunked transfer encoding
- [ ] Partial send handling
- [ ] Request timeouts
- [ ] Config file support
- [ ] Thread pool + event loop hybrid
- [ ] HTTP pipelining
- [ ] HTTPS / TLS support

---

## Learning Goals

This project was built to deeply understand:

- Low-level networking
- Event-driven server architecture
- TCP stream behavior
- HTTP protocol internals
- Kernel I/O multiplexing
- Scalable backend systems

---

## License

MIT License — free to use, modify, and distribute.