#ifndef REACTOR_INET_ADDRESS_H
#define REACTOR_INET_ADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
private:
    sockaddr_in addr_{};
public:
    InetAddress(const std::string &ip, uint16_t port);
    InetAddress(unsigned int ip, uint16_t port);
    InetAddress(const sockaddr_in addr);
    ~InetAddress() = default;

    const char *ip() const;
    uint16_t    port() const;
    const sockaddr *addr() const;
};

#endif //REACTOR_INET_ADDRESS_H
