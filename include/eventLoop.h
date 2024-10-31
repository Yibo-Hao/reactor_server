//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include <thread>
#include <utility>

#include "epoll.h"
#include "channel.h"

class Epoll;
class Channel;

class EventLoop {
private:
    std::unique_ptr<Epoll> ep_;
    std::function<void(EventLoop*)> epoll_timeout_callback_;
public:
    EventLoop();
    ~EventLoop();

    void run();
    void update_channel(Channel *ch);
    void remove_channel(Channel *ch);
    void set_epoll_timeout_callback(std::function<void(EventLoop*)> fn);
};

#endif //REACTOR_EVENTLOOP_H
