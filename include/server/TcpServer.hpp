#pragma once

#include <string>

class TcpServer
{
private:
    int server_fd;
    int port;

public:
    TcpServer(int port);
    ~TcpServer();

    void start();
};