//
// Created by 郝绎博 on 2024/10/25.
//
#include "channel.h"

#include <utility>

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd)
{
}

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
    loop_->update_channel(this);
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
    read_callback_ = std::move(fn);
}
