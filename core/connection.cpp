//
// Created by 郝绎博 on 2024/10/26.
//
#include "connection.h"

Connection::Connection(EventLoop *loop, Socket *client_socket) : loop_(loop), client_socket_(client_socket)
{
    client_channel_ = new Channel(loop_, client_socket_->fd());
    client_channel_->set_read_callback(std::bind(&Connection::on_message, this));
    client_channel_->set_error_callback(std::bind(&Connection::close_callback, this));
    client_channel_->set_close_callback(std::bind(&Connection::error_callback, this));
    client_channel_->set_write_callback(std::bind(&Connection::write_callback, this));
    client_channel_->enablereading();
    client_channel_->useet();
}

Connection::~Connection()
{
    delete client_channel_;
    delete client_socket_;
}

int Connection::fd() const
{
    return client_socket_->fd();
}

std::string Connection::ip() const
{
    return client_socket_->ip();
}

uint16_t Connection::port() const
{
    return client_socket_->port();
}

void Connection::close_callback()
{
    close_callback_(shared_from_this());
}

void Connection::error_callback()
{
    error_callback_(shared_from_this());
}

void Connection::set_close_callback(const std::function<void(spConnection)> &cb)
{
    close_callback_ = cb;
}

void Connection::set_error_callback(const std::function<void(spConnection)> &cb)
{
    error_callback_ = cb;
}

void Connection::set_message_callback(const std::function<void(spConnection, std::string&)> &cb)
{
    message_callback_ = cb;
}

void Connection::set_send_complete_callback(const std::function<void(spConnection)> &cb)
{
    send_complete_callback_ = cb;
}

void Connection::on_message()
{
    char buffer[1024];
    while (true)
    {
        memset(&buffer, 0, sizeof(buffer));
        ssize_t n_read = recv(fd(), buffer, sizeof(buffer), 0);
        if (n_read > 0)
        {
            input_buffer_.append(buffer, n_read);
        }
        else if (n_read == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (n_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            while (true) {
                int len;
                memcpy(&len, input_buffer_.data(), 4);
                if (input_buffer_.size() < len + 4) {
                    break;
                }
                std::string message(input_buffer_.data() + 4, len);
                input_buffer_.erase(0, len + 4);
                message_callback_(shared_from_this(), message);
            }
            break;
        }
        else if (n_read == 0)
        {
            close_callback();
            break;
        }
    }
}

void Connection::send(const char* message, size_t len)
{
    output_buffer_.append_with_head(message, len);
    client_channel_->enablewriting();
}

void Connection::write_callback()
{
    ssize_t n_write = ::send(fd(), output_buffer_.data(), output_buffer_.size(), 0);
    if (n_write > 0)
    {
        output_buffer_.erase(0, n_write);
    }

    if (output_buffer_.size() == 0)
    {
        client_channel_->disablewriting();
        send_complete_callback_(shared_from_this());
    }
}