//
// Created by 郝绎博 on 2024/10/25.
//
#include "channel.h"

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