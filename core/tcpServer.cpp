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

    for (auto &connection : connections_) {
        delete connection.second;
    }
}

void TcpServer::start() {
    loop_->run();
}

void TcpServer::new_connection(Socket *client_socket) {
    Connection *connection = new Connection(loop_, client_socket);
    connection->set_close_callback(std::bind(&TcpServer::close_connection, this, std::placeholders::_1));
    connection->set_error_callback(std::bind(&TcpServer::close_connection, this, std::placeholders::_1));
    std::cout << "New client connected: fd:" << connection->fd() << " IP:" << connection->ip() << ":" << connection->port() << std::endl;
    connections_[connection->fd()] = connection;
}

void TcpServer::close_connection(Connection* connection) {
    std::cout << "Client disconnected: " << connection->fd() << std::endl;
    connections_.erase(connection->fd());
    delete connection;
}

void TcpServer::error_connection(Connection* connection) {
    std::cout << "Client error: " << connection->fd() << std::endl;
    connections_.erase(connection->fd());
    delete connection;
}