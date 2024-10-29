//
// Created by 郝绎博 on 2024/10/28.
//
#include "echoServer.h"

EchoServer::EchoServer(const std::string &ip, const uint16_t &port, int sub_thread_num, int work_thread_num)
: tcp_server_(ip, port, sub_thread_num), thread_pool_(work_thread_num)
{
    tcp_server_.setnewconnectioncb(std::bind(&EchoServer::handle_new_connection, this, std::placeholders::_1));
    tcp_server_.setcloseconnectioncb(std::bind(&EchoServer::handle_close_connection, this, std::placeholders::_1));
    tcp_server_.seterrorconnectioncb(std::bind(&EchoServer::handle_error_connection, this, std::placeholders::_1));
    tcp_server_.setonmessagecb(std::bind(&EchoServer::handle_message_connection, this, std::placeholders::_1, std::placeholders::_2));
    tcp_server_.setsendcompletecb(std::bind(&EchoServer::handle_message_complete, this, std::placeholders::_1));
}

EchoServer::~EchoServer() = default;

void EchoServer::start()
{
    tcp_server_.start();
}

void EchoServer::handle_new_connection(Connection *connection)
{
    std::cout << "New connection." << std::endl;
}

void EchoServer::handle_close_connection(Connection* connection)
{
    std::cout << "Connection closed." << std::endl;
}

void EchoServer::handle_error_connection(Connection* connection)
{
    std::cout << "Connection error." << std::endl;
}

void EchoServer::handle_message_connection(Connection* connection, std::string &message)
{
    thread_pool_.addtask(std::bind(&EchoServer::on_message, this, connection, message));
}

void EchoServer::handle_message_complete(Connection* connection)
{
    std::cout << "Message send complete." << std::endl;
}

void EchoServer::handle_epoll_timeout(EventLoop *loop)
{
    std::cout << "Epoll timeout." << std::endl;
}

void EchoServer::on_message(Connection *connection, std::string &message)
{
    std::cout << "recv: " << message << std::endl;
    message = "reply:" + message;
    connection->send(message.data(), message.size());
}