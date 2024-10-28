//
// Created by 郝绎博 on 2024/10/26.
//
#include "tcpServer.h"

#include <utility>

TcpServer::TcpServer(const std::string &ip, const uint16_t &port) {
    loop_ = new EventLoop;
    loop_->set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
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
    connection->set_message_callback(std::bind(&TcpServer::message_connection, this, std::placeholders::_1, std::placeholders::_2));
    connection->set_send_complete_callback(std::bind(&TcpServer::message_complete, this, std::placeholders::_1));
    connections_[connection->fd()] = connection;
    if (newconnectioncb_) newconnectioncb_(connection);
}

void TcpServer::close_connection(Connection* connection) {
    if (closeconnectioncb_) closeconnectioncb_(connection);
    connections_.erase(connection->fd());
    delete connection;
}

void TcpServer::error_connection(Connection* connection) {
    if (errorconnectioncb_) errorconnectioncb_(connection);
    connections_.erase(connection->fd());
    delete connection;
}

void TcpServer::message_connection(Connection* connection, std::string message) {
    if (onmessagecb_) onmessagecb_(connection, message);
}

void TcpServer::message_complete(Connection* connection) {
    if (sendcompletecb_) sendcompletecb_(connection);
}

void TcpServer::epoll_timeout(EventLoop *loop) {
    if (timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(Connection*)> fn)
{
    newconnectioncb_ = std::move(fn);
}

void TcpServer::setcloseconnectioncb(std::function<void(Connection*)> fn)
{
    closeconnectioncb_ = std::move(fn);
}

void TcpServer::seterrorconnectioncb(std::function<void(Connection*)> fn)
{
    errorconnectioncb_ = std::move(fn);
}

void TcpServer::setonmessagecb(std::function<void(Connection*,std::string &message)> fn)
{
    onmessagecb_ = std::move(fn);
}

void TcpServer::setsendcompletecb(std::function<void(Connection*)> fn)
{
    sendcompletecb_ = std::move(fn);
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_ = std::move(fn);
}