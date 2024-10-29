//
// Created by 郝绎博 on 2024/10/28.
//

#ifndef REACTOR_ECHO_SERVER_H
#define REACTOR_ECHO_SERVER_H

#include "tcpServer.h"
#include "threadPool.h"

class EchoServer {
private:
    TcpServer tcp_server_;
    ThreadPool thread_pool_;
public:
    EchoServer(const std::string &ip, const uint16_t &port, int sub_thread_num = 3, int work_thread_num = 5);
    ~EchoServer();

    void start();
    void handle_new_connection(Connection *connection);
    void handle_close_connection(Connection* connection);
    void handle_error_connection(Connection* connection);
    void handle_message_connection(Connection* connection, std::string &message);
    void handle_message_complete(Connection* connection);
    void handle_epoll_timeout(EventLoop *loop);
    void on_message(Connection *conn, std::string &message);
};

#endif //REACTOR_ECHO_SERVER_H