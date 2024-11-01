//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_CONNECTION_H
#define REACTOR_CONNECTION_H

#include <string>
#include <memory>
#include <atomic>

#include "eventLoop.h"
#include "socket.h"
#include "channel.h"
#include "functional"
#include "reactorBuffer.h"

class Channel;
class EventLoop;
class Connection;

using spConnection = std::shared_ptr<Connection>;

class Connection : public std::enable_shared_from_this<Connection> {
private:
    EventLoop *loop_;
    std::unique_ptr<Socket> client_socket_;
    std::unique_ptr<Channel> client_channel_;
    Buffer input_buffer_;
    Buffer output_buffer_;
    std::atomic<bool> disconnect_;

    std::function<void(spConnection)> close_callback_;
    std::function<void(spConnection)> error_callback_;
    std::function<void(spConnection, std::string&)> message_callback_;
    std::function<void(spConnection)> send_complete_callback_;
public:
    Connection(EventLoop *loop, std::unique_ptr<Socket> client_socket);
    ~Connection();

    int fd() const;
    std::string ip() const;
    uint16_t port() const;

    void on_message();
    void send(const char* message, size_t len);
    void send_in_poll(const char* message, size_t len);
    void close_callback();
    void error_callback();
    void write_callback();

    void set_close_callback(const std::function<void(spConnection)> &cb);
    void set_error_callback(const std::function<void(spConnection)> &cb);
    void set_message_callback(const std::function<void(spConnection, std::string&)> &cb);
    void set_send_complete_callback(const std::function<void(spConnection)> &cb);
};

#endif //REACTOR_CONNECTION_H
