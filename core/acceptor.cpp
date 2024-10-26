//
// Created by 郝绎博 on 2024/10/26.
//
#include "acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const std::string &ip, const uint16_t &port) : loop_(loop) {
    server_socket_ = new Socket(create_nonblocking());
    InetAddress server_addr(ip, port);
    server_socket_->set_reuse_addr(true);
    server_socket_->set_reuse_port(true);
    server_socket_->set_tcp_no_delay(true);
    server_socket_->set_keepalive(true);
    server_socket_->bind(server_addr);
    server_socket_->listen(128);
    std::cout << "Server started." << std::endl;

    // epoll
    accept_channel_ = new Channel(loop_->ep(), server_socket_->fd());
    accept_channel_->set_read_callback(std::bind(&Channel::new_connection, accept_channel_, server_socket_));
    accept_channel_->enablereading();
}

Acceptor::~Acceptor() {
    delete server_socket_;
    delete accept_channel_;
}