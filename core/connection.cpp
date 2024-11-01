//
// Created by 郝绎博 on 2024/10/26.
//
#include "connection.h"

Connection::Connection(EventLoop *loop, std::unique_ptr<Socket> client_socket)
: loop_(loop), client_socket_(std::move(client_socket)),
client_channel_(std::make_unique<Channel>(loop_, client_socket_->fd())), disconnect_(false)
{
    client_channel_->set_read_callback([this] { on_message(); });
    client_channel_->set_error_callback([this] { close_callback(); });
    client_channel_->set_close_callback([this] { error_callback(); });
    client_channel_->set_write_callback([this] { write_callback(); });
    client_channel_->enablereading();
    client_channel_->useet();
}

Connection::~Connection()
= default;

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
    disconnect_.store(true);
    client_channel_->remove();
    close_callback_(shared_from_this());
}

void Connection::error_callback()
{
    disconnect_.store(true);
    client_channel_->remove();
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
    if (disconnect_.load())
    {
        std::cout << "connection has been closed" << std::endl;
        return;
    }

    if (loop_->is_in_loop_thread())
    {
        std::cout << "send in IO thread: " << ::syscall(SYS_gettid) << std::endl;
        send_in_poll(message, len);
    }
    else
    {
        std::cout << "send in work thread: " << ::syscall(SYS_gettid) << std::endl;
        loop_->queue_in_loop(std::bind(&Connection::send_in_poll, this, message, len));
    }
}

void Connection::send_in_poll(const char* message, size_t len)
{
    std::cout << "send in thread: " << ::syscall(SYS_gettid) << std::endl;
    output_buffer_.append_with_head(message, len);
    client_channel_->enablewriting();
}

void Connection::write_callback()
{
    std::cout << "write_callback in IO thread: " << ::syscall(SYS_gettid) << std::endl;
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