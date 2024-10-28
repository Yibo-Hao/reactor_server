//
// Created by 郝绎博 on 2024/10/26.
//
#include "eventLoop.h"

#include <utility>

EventLoop::EventLoop()
{
    ep_ = new Epoll();
}

EventLoop::~EventLoop()
{
    delete ep_;
}

void EventLoop::run()
{
    while(true)
    {
        std::vector<Channel*> channels = ep_->loop(10*1000);
        if (channels.empty()) {
            epoll_timeout_callback_(this);
        }
        std::for_each(channels.begin(), channels.end(), [](Channel* channel) {
            channel->handle_event();
        });
    }
}

Epoll *EventLoop::ep() const
{
    return ep_;
}

void EventLoop::update_channel(Channel *ch)
{
    ep_->update_channel(ch);
}

void EventLoop::set_epoll_timeout_callback(std::function<void(EventLoop*)> fn)
{
    epoll_timeout_callback_ = std::move(fn);
}