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
#include "reactorBuffer.h"

class Channel;
class EventLoop;

class Connection {
private:
    EventLoop *loop_;
    Socket *client_socket_;
    Channel *client_channel_;
    Buffer input_buffer_;
    Buffer output_buffer_;

    std::function<void(Connection*)> close_callback_;
    std::function<void(Connection*)> error_callback_;
    std::function<void(Connection*, std::string)> message_callback_;
public:
    Connection(EventLoop *loop, Socket* client_socket);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void on_message();
    void send(const char* message, size_t len);
    void close_callback();
    void error_callback();
    void write_callback();

    void set_close_callback(const std::function<void(Connection*)> &cb);
    void set_error_callback(const std::function<void(Connection*)> &cb);
    void set_message_callback(const std::function<void(Connection*, std::string)> &cb);
};

#endif //REACTOR_CONNECTION_H
