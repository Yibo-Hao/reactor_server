//
// Created by 郝绎博 on 2024/10/25.
//

#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include <sys/epoll.h>
#include <memory>

#include "inetAddress.h"
#include "socket.h"
#include "epoll.h"

class Epoll;

class Channel {
private:
    int fd_ = -1; // Channel 拥有的 fd，一对一
    Epoll *ep_ = nullptr; // Channel 对应的红黑树，多对一
    bool inepoll_ = false; // 是否已经添加到 epoll 树中
    uint32_t events_ = 0;
    uint32_t revents_ = 0;
public:
    Channel(Epoll *ep, int fd);
    ~Channel();

    int fd() const;
    void useet(); // 采用边缘触发
    void enablereading();
    void setinepoll();
    void setrevents(uint32_t ev);
    bool inpoll() const;
    uint32_t events() const;
    uint32_t revents() const;
};


#endif //REACTOR_CHANNEL_H
