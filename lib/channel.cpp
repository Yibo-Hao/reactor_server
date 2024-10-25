//
// Created by 郝绎博 on 2024/10/25.
//
#include "channel.h"

#include <utility>

Channel::Channel(Epoll *ep, int fd) : ep_(ep), fd_(fd)
{
}

// 在析构函数中，不要销毁ep_，也不能关闭fd_，这两个东西不属于Channel类，Channel类只是使用它们。
Channel::~Channel() = default;

int Channel::fd() const
{
    return fd_;
}

void Channel::useet()
{
    events_ = events_ | EPOLLET;
}

void Channel::enablereading()
{
    events_ = events_ | EPOLLIN;
    ep_->update_channel(this);
}

void Channel::setinepoll()
{
    inepoll_ = true;
}

void Channel::setrevents(uint32_t ev)
{
    revents_ = ev;
}

bool Channel::inpoll() const
{
    return inepoll_;
}

uint32_t Channel::events() const
{
    return events_;
}

uint32_t Channel::revents() const
{
    return revents_;
}

void Channel::handle_event()
{
    if (revents_ & EPOLLRDHUP)
    {
        std::cout << "Client disconnected: " << fd_ << std::endl;
        close(fd_);
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        read_callback_();
    }
    else
    {
        std::cout << "Client error: " << fd_ << std::endl;
        close(fd_);
    }
}

void Channel::new_connection(std::shared_ptr<Socket>& server_socket)
{
    InetAddress client_addr{};
    int client_fd = server_socket->accept(client_addr);
    std::cout << "New client connected: " << client_fd << client_addr.ip() << client_addr.port() << std::endl;
    Channel *client_channel = new Channel(ep_, client_fd);
    client_channel->set_read_callback(std::bind(&Channel::on_message, client_channel));
    client_channel->enablereading();
    client_channel->useet();
}

void Channel::on_message()
{
    char buffer[1024];
    while (true)
    {
        memset(&buffer, 0, sizeof(buffer));
        ssize_t n_read = recv(fd_, buffer, sizeof(buffer), 0);
        if (n_read > 0)
        {
            std::cout << "recv: " << buffer << std::endl;
            send(fd_, "OK", 2, 0);
        }
        else if (n_read == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
        {
            continue;
        }
        else if (n_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
        {
            break;
        }
        else if (n_read == 0)
        {
            std::cout << "Client disconnected: " << fd_ << std::endl;
            close(fd_);
            break;
        }
    }
}

void Channel::set_read_callback(std::function<void()> fn)
{
    read_callback_ =  std::move(fn);
}
