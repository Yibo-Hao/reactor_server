//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_EVENTLOOP_H
#define REACTOR_EVENTLOOP_H

#include "epoll.h"
#include "channel.h"

class EventLoop {
private:
    Epoll *ep_;
public:
    EventLoop();
    ~EventLoop();

    void run();
    Epoll *ep() const;
};

#endif //REACTOR_EVENTLOOP_H
