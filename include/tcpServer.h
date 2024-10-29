//
// Created by 郝绎博 on 2024/10/26.
//

#ifndef REACTOR_TCPSERVER_H
#define REACTOR_TCPSERVER_H

#include <string>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <map>
#include <utility>

#include "channel.h"
#include "eventLoop.h"
#include "inetAddress.h"
#include "acceptor.h"
#include "connection.h"
#include "threadPool.h"

class TcpServer {
private:
    EventLoop *main_loop_;
    std::vector<EventLoop*> sub_loops_;
    ThreadPool *thread_pool_;
    int thread_num_;
    Acceptor *acceptor_;
    std::map <int, Connection *> connections_;
    std::function<void(Connection*)> newconnectioncb_;
    std::function<void(Connection*)> closeconnectioncb_;
    std::function<void(Connection*)> errorconnectioncb_;
    std::function<void(Connection*,std::string &message)> onmessagecb_;
    std::function<void(Connection*)> sendcompletecb_;
    std::function<void(EventLoop*)>  timeoutcb_;
public:
    TcpServer(const std::string &ip, const uint16_t &port, int thread_num = 3);
    ~TcpServer();

    void start();
    void new_connection(Socket *client_socket);
    void close_connection(Connection* connection);
    void error_connection(Connection* connection);
    void message_connection(Connection* connection, std::string &message);
    void message_complete(Connection* connection);
    void epoll_timeout(EventLoop *loop);

    void setnewconnectioncb(std::function<void(Connection*)> fn);
    void setcloseconnectioncb(std::function<void(Connection*)> fn);
    void seterrorconnectioncb(std::function<void(Connection*)> fn);
    void setonmessagecb(std::function<void(Connection*,std::string &message)> fn);
    void setsendcompletecb(std::function<void(Connection*)> fn);
    void settimeoutcb(std::function<void(EventLoop*)> fn);
};

#endif //REACTOR_TCPSERVER_H
