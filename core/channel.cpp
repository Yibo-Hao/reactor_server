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

void Channel::disablereading() {
    events_ = events_ & ~EPOLLIN;
    loop_->update_channel(this);
}

void Channel::enablewriting()
{
    events_ = events_ | EPOLLOUT;
    loop_->update_channel(this);
}

void Channel::disablewriting() {
    events_ = events_ & ~EPOLLOUT;
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
        remove();
        close_callback_();
    }
    else if (revents_ & (EPOLLIN | EPOLLPRI))
    {
        read_callback_();
    }
    else if (revents_ & EPOLLOUT)
    {
        write_callback_();
    }
    else
    {
        remove();
        error_callback_();
    }
}

void Channel::set_read_callback(std::function<void()> fn)
{
    read_callback_ = std::move(fn);
}

void Channel::set_close_callback(std::function<void()> fn)
{
    close_callback_ = std::move(fn);
}

void Channel::set_error_callback(std::function<void()> fn)
{
    error_callback_ = std::move(fn);
}

void Channel::set_write_callback(std::function<void()> fn)
{
    write_callback_ = std::move(fn);
}

void Channel::disableall()
{
    events_ = 0;
    loop_->update_channel(this);
}

void Channel::remove()
{
    disableall();
    loop_->remove_channel(this);
}
