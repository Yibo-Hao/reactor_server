//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_CONNECTION_H
#define REACTOR_CONNECTION_H

#include <string>

#include "eventLoop.h"
#include "socket.h"
#include "channel.h"
#include "functional"

class Channel;
class EventLoop;

class Connection {
private:
    EventLoop *loop_;
    Socket *client_socket_;
    Channel *client_channel_;
public:
    Connection(EventLoop *loop, Socket* client_socket);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;
};

#endif //REACTOR_CONNECTION_H
