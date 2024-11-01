//
// Created by 郝绎博 on 2024/10/26.
//
#include "tcpServer.h"

TcpServer::TcpServer(const std::string &ip, const uint16_t &port, int thread_num)
: thread_num_(thread_num), main_loop_(std::make_unique<EventLoop>(true)), acceptor_(main_loop_.get(), ip, port),
thread_pool_(thread_num)
{
    main_loop_->set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
    acceptor_.set_new_connection_callback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1));
    for (int i = 0; i < thread_num_; ++i)
    {
        sub_loops_.emplace_back(new EventLoop(false, 5, 10));
        sub_loops_[i]->set_timer_callback(std::bind(&TcpServer::remove_connection, this, std::placeholders::_1));
        sub_loops_[i]->set_epoll_timeout_callback(std::bind(&TcpServer::epoll_timeout, this, std::placeholders::_1));
        thread_pool_.addtask(std::bind(&EventLoop::run, sub_loops_[i].get()));
    }
}

TcpServer::~TcpServer()
= default;

void TcpServer::start()
{
    main_loop_->run();
}

void TcpServer::stop()
{
    main_loop_->stop();
    for (int i = 0; i < thread_num_; ++i)
    {
        sub_loops_[i]->stop();
    }
    thread_pool_.stop();
}

void TcpServer::new_connection(std::unique_ptr<Socket> client_socket)
{
    spConnection connection = std::make_shared<Connection>(sub_loops_[client_socket->fd() % thread_num_].get(), std::move(client_socket));
    connection->set_close_callback(std::bind(&TcpServer::close_connection, this, std::placeholders::_1));
    connection->set_error_callback(std::bind(&TcpServer::close_connection, this, std::placeholders::_1));
    connection->set_message_callback(std::bind(&TcpServer::message_connection, this, std::placeholders::_1, std::placeholders::_2));
    connection->set_send_complete_callback(std::bind(&TcpServer::message_complete, this, std::placeholders::_1));
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_[connection->fd()] = connection;
    }
    sub_loops_[connection->fd() % thread_num_]->new_connection(connection);
    if (newconnectioncb_) newconnectioncb_(connection);
}

void TcpServer::close_connection(spConnection connection)
{
    if (closeconnectioncb_) closeconnectioncb_(connection);
    remove_connection(connection->fd());
}

void TcpServer::error_connection(spConnection connection) {
    if (errorconnectioncb_) errorconnectioncb_(connection);
    remove_connection(connection->fd());
}

void TcpServer::message_connection(spConnection connection, std::string &message) {
    if (onmessagecb_) onmessagecb_(connection, message);
}

void TcpServer::message_complete(spConnection connection) {
    if (sendcompletecb_) sendcompletecb_(connection);
}

void TcpServer::epoll_timeout(EventLoop *loop) {
    if (timeoutcb_) timeoutcb_(loop);
}

void TcpServer::setnewconnectioncb(std::function<void(spConnection)> fn)
{
    newconnectioncb_ = std::move(fn);
}

void TcpServer::setcloseconnectioncb(std::function<void(spConnection)> fn)
{
    closeconnectioncb_ = std::move(fn);
}

void TcpServer::seterrorconnectioncb(std::function<void(spConnection)> fn)
{
    errorconnectioncb_ = std::move(fn);
}

void TcpServer::setonmessagecb(std::function<void(spConnection, std::string &message)> fn)
{
    onmessagecb_ = std::move(fn);
}

void TcpServer::setsendcompletecb(std::function<void(spConnection)> fn)
{
    sendcompletecb_ = std::move(fn);
}

void TcpServer::settimeoutcb(std::function<void(EventLoop*)> fn)
{
    timeoutcb_ = std::move(fn);
}

void TcpServer::remove_connection(int fd)
{
    std::lock_guard<std::mutex> lock(mutex_);
    connections_.erase(fd);
}