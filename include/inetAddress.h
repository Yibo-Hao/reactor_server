#ifndef REACTOR_INETADDRESS_H
#define REACTOR_INETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in addr_{};
public:
    InetAddress() = default;
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(unsigned int ip, uint16_t port);
    explicit InetAddress(sockaddr_in addr);
    ~InetAddress() = default;

    const char *ip() const;
    uint16_t    port() const;
    const sockaddr *addr() const;
    void set_addr(sockaddr_in client_addr);
};

#endif //REACTOR_INETADDRESS_H
