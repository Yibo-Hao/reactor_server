//
// Created by 郝绎博 on 2024/10/26.
//
#include "tcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t &port) : loop_(new EventLoop){
    auto *server_socket = new Socket(create_nonblocking());
    InetAddress server_addr(INADDR_ANY, port);
    server_socket->set_reuse_addr(true);
    server_socket->set_reuse_port(true);
    server_socket->set_tcp_no_delay(true);
    server_socket->set_keepalive(true);
    server_socket->bind(server_addr);
    server_socket->listen(128);
    std::cout << "Server started." << std::endl;

    // epoll
    Channel *server_channel = new Channel(loop_->ep(), server_socket->fd());
    server_channel->set_read_callback(std::bind(&Channel::new_connection, server_channel, server_socket));
    server_channel->enablereading();
}

TcpServer::~TcpServer() {
    delete loop_;
}

void TcpServer::start() {
    loop_->run();
}