
#ifndef REACTOR_EPOLL_H
#define REACTOR_EPOLL_H

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <strings.h>
#include <cstring>
#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include <unistd.h>
#include <memory>

#include "channel.h"

class Channel;
class EventLoop;

class Epoll
{
private:
    static const int MaxEvents = 100;
    int epoll_fd_{-1};
    epoll_event events_[MaxEvents]{};
    std::function<void(EventLoop*)> timeout_callback_;
public:
    Epoll();
    ~Epoll();

    void update_channel(Channel*) const;
    void remove_channel(Channel*) const;
    std::vector<Channel *> loop(int timeout = -1);
    void set_timeout_callback(const std::function<void(EventLoop*)> &cb);
};

#endif
