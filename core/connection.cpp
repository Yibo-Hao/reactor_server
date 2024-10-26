//
// Created by 郝绎博 on 2024/10/26.
//
#include "connection.h"

Connection::Connection(EventLoop *loop, Socket *client_socket) : loop_(loop), client_socket_(client_socket) {
    client_channel_ = new Channel(loop_, client_socket_->fd());
    client_channel_->set_read_callback(std::bind(&Channel::on_message, client_channel_));
    client_channel_->enablereading();
    client_channel_->useet();
}

Connection::~Connection() {
    delete client_channel_;
    delete client_socket_;
}