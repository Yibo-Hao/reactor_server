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

#include "channel.h"
#include "eventLoop.h"
#include "inetAddress.h"
#include "acceptor.h"
#include "connection.h"

class TcpServer {
private:
    EventLoop *loop_;
    Acceptor *acceptor_;
    std::map <int, Connection *> connections_;
public:
    TcpServer(const std::string &ip, const uint16_t &port);
    ~TcpServer();

    void start();
    void new_connection(Socket *client_socket);
    void close_connection(Connection* connection);
    void error_connection(Connection* connection);
    void message_connection(Connection* connection, std::string message);
    void message_complete(Connection* connection);
    void epoll_timeout(EventLoop *loop);
};

#endif //REACTOR_TCPSERVER_H
