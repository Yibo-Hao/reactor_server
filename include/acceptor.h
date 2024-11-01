 //
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_ACCEPTOR_H
#define REACTOR_ACCEPTOR_H

#include <string>
#include <functional>

#include "eventLoop.h"
#include "socket.h"
#include "channel.h"
#include "connection.h"

class Acceptor {
private:
    EventLoop *loop_;
    Socket server_socket_;
    Channel accept_channel_;
    std::function<void(std::unique_ptr<Socket>)> new_connection_callback_;
public:
    Acceptor(EventLoop *loop, const std::string &ip, const uint16_t &port);
    ~Acceptor();

    void new_connection();
    void set_new_connection_callback(const std::function<void(std::unique_ptr<Socket>)> &cb);
};

#endif //REACTOR_ACCEPTOR_H
