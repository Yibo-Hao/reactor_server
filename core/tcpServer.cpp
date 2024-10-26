//
// Created by 郝绎博 on 2024/10/26.
//
#include "tcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t &port) {
    loop_ = new EventLoop;
    acceptor_ = new Acceptor(loop_, ip, port);
    acceptor_->set_new_connection_callback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1));
}

TcpServer::~TcpServer() {
    delete loop_;
    delete acceptor_;
}

void TcpServer::start() {
    loop_->run();
}

void TcpServer::new_connection(Socket *client_socket) {
    Connection *connection = new Connection(loop_, client_socket);
}