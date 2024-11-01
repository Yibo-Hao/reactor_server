//
// Created by 郝绎博 on 2024/10/26.
//
#include "acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t &port)
: loop_(loop), server_socket_(create_nonblocking()), accept_channel_(loop_, server_socket_.fd())
{
    InetAddress server_addr(ip, port);
    server_socket_.set_reuse_addr(true);
    server_socket_.set_reuse_port(true);
    server_socket_.set_tcp_no_delay(true);
    server_socket_.set_keepalive(true);
    server_socket_.bind(server_addr);
    server_socket_.listen(128);
    std::cout << "Server started." << std::endl;

    accept_channel_.set_read_callback([this] { new_connection(); });
    accept_channel_.enablereading();
}

Acceptor::~Acceptor()
= default;

void Acceptor::new_connection()
{
    InetAddress client_addr{};
    std::unique_ptr<Socket> client_socket = std::make_unique<Socket>(server_socket_.accept(client_addr));
    client_socket->set_ip_port(client_addr.ip(), client_addr.port());
    new_connection_callback_(std::move(client_socket));
}

void Acceptor::set_new_connection_callback(const std::function<void(std::unique_ptr<Socket>)> &cb)
{
    new_connection_callback_ = cb;
}