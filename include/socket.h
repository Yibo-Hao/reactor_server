#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <unistd.h>
#include <string>

#include "inetAddress.h"

// 创建一个非阻塞的socket。
int create_nonblocking();

class Socket
{
private:
    const int fd_;
    std::string ip_;
    uint16_t port_;
public:
    explicit Socket(int fd);
    ~Socket();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
    void set_ip_port(const std::string& ip, uint16_t port);

    void set_reuse_addr(bool on);
    void set_reuse_port(bool on);
    void set_tcp_no_delay(bool on);
    void set_keepalive(bool on);
    void bind(const InetAddress& server_addr);
    void listen(int nn=128);
    int accept(InetAddress& client_addr);
};