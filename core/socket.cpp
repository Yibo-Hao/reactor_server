#include "socket.h"

// 创建一个非阻塞的socket。
int create_nonblocking()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
    if (listen_fd < 0)
    {
        std::cerr << "socket() failed" << __FILE__ << __FUNCTION__ << __LINE__ << std::endl;
        exit(-1);
    }
    return listen_fd;
}

Socket::Socket(int fd) : fd_(fd)
{

}

Socket::~Socket()
{
    ::close(fd_);
}

int Socket::fd() const
{
    return fd_;
}

std::string Socket::ip() const
{
    return ip_;
}

uint16_t Socket::port() const
{
    return port_;
}

void Socket::set_tcp_no_delay(bool on)
{
    int val = on ? 1 : 0;
    ::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
}

void Socket::set_reuse_addr(bool on)
{
    int val = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

void Socket::set_reuse_port(bool on)
{
    int val = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &val, sizeof(val));
}

void Socket::set_keepalive(bool on)
{
    int val = on ? 1 : 0;
    ::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));
}

void Socket::bind(const InetAddress& serv_addr)
{
    if (::bind(fd_, serv_addr.addr(), sizeof(sockaddr)) < 0 )
    {
        std::cerr << "bind() failed" << std::endl;
        close(fd_);
        exit(-1);
    }
    ip_ = serv_addr.ip();
    port_ = serv_addr.port();
}

void Socket::listen(int nn)
{
    if (::listen(fd_, nn) != 0 )
    {
        std::cerr << "listen() failed" << std::endl;
        close(fd_);
        exit(-1);
    }
}

int Socket::accept(InetAddress& client_addr)
{
    sockaddr_in peer_addr{};
    socklen_t len = sizeof(peer_addr);
    int client_fd = accept4(fd_, (sockaddr*)&peer_addr, &len, SOCK_NONBLOCK);

    client_addr.set_addr(peer_addr);

    ip_ = client_addr.ip();
    port_ = client_addr.port();

    return client_fd;
}