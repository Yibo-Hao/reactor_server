#include "inetAddress.h"

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(unsigned int ip, uint16_t port)
{
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = ip;
    addr_.sin_port = htons(port);
}

InetAddress::InetAddress(sockaddr_in addr): addr_(addr)
{

}

const char *InetAddress::ip() const
{
    return inet_ntoa(addr_.sin_addr);
}

uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr *InetAddress::addr() const
{
    return (sockaddr*)(&addr_);
}

void InetAddress::set_addr(sockaddr_in client_addr)
{
    addr_ = client_addr;
}