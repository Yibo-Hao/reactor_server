//
// Created by 郝绎博 on 2024/10/26.
//
#include "connection.h"

Connection::Connection(EventLoop *loop, Socket *client_socket) : loop_(loop), client_socket_(client_socket) {
    client_channel_ = new Channel(loop_, client_socket_->fd());
    client_channel_->set_read_callback(std::bind(&Channel::on_message, client_channel_));
    client_channel_->set_error_callback(std::bind(&Connection::close_callback, this));
    client_channel_->set_close_callback(std::bind(&Connection::error_callback, this));
    client_channel_->enablereading();
    client_channel_->useet();
}

Connection::~Connection() {
    delete client_channel_;
    delete client_socket_;
}

int Connection::fd() const {
    return client_socket_->fd();
}

std::string Connection::ip() const {
    return client_socket_->ip();
}

uint16_t Connection::port() const {
    return client_socket_->port();
}

void Connection::close_callback() {
    close_callback_(this);
}

void Connection::error_callback() {
    error_callback_(this);
}

void Connection::set_close_callback(const std::function<void(Connection *)> &cb) {
    close_callback_ = cb;
}

void Connection::set_error_callback(const std::function<void(Connection *)> &cb) {
    error_callback_ = cb;
}