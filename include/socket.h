#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <unistd.h>

#include "inetAddress.h"

// 创建一个非阻塞的socket。
int create_nonblocking();

class Socket
{
private:
    const int fd_;
public:
    explicit Socket(int fd);
    ~Socket();

    int fd() const;
    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void set_tcp_no_delay(bool on);
    void set_keepalive(bool on);
    void bind(const InetAddress& server_addr);
    void listen(int nn=128);
    int accept(InetAddress& client_addr);
};