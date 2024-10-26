//
// Created by 郝绎博 on 2024/10/25.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include <sys/epoll.h>
#include <memory>
#include <functional>

#include "inetAddress.h"
#include "socket.h"
#include "epoll.h"
#include "connection.h"
#include "eventLoop.h"

class EventLoop;

class Channel {
private:
    int fd_ = -1; // Channel 拥有的 fd，一对一
    EventLoop *loop_ = nullptr;
    bool inepoll_ = false; // 是否已经添加到 epoll 树中
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
    std::function<void()> read_callback_;
public:
    Channel(EventLoop *loop, int fd);
    ~Channel();

    int fd() const;
    void useet(); // 采用边缘触发
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool inpoll() const;
    uint32_t events() const;
    uint32_t revents() const;
    void handle_event();
    void on_message();
    void set_read_callback(std::function<void()> fn);
};


#endif //REACTOR_CHANNEL_H
