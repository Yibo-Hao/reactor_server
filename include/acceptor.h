 //
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_ACCEPTOR_H
#define REACTOR_ACCEPTOR_H

#include <string>

#include "eventLoop.h"
#include "socket.h"
#include "channel.h"
#include "functional"
#include "connection.h"

class Acceptor {
private:
    EventLoop *loop_;
    Socket *server_socket_;
    Channel *accept_channel_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t &port);
    ~Acceptor();

    void new_connection();
};

#endif //REACTOR_ACCEPTOR_H
