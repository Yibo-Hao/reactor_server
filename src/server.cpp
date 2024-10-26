#include <iostream>
#include <netinet/in.h>
#include <memory>

#include "inetAddress.h"
#include "socket.h"
#include "epoll.h"
#include "channel.h"
#include "eventLoop.h"

int PORT = 7175;

int main() {
    // socket
    std::shared_ptr<Socket> server_socket = std::make_shared<Socket>(create_nonblocking());
    InetAddress server_addr(INADDR_ANY, PORT);
    int listened_fd = server_socket->fd();
    server_socket->set_reuse_addr(true);
    server_socket->set_reuse_port(true);
    server_socket->set_tcp_no_delay(true);
    server_socket->set_keepalive(true);
    server_socket->bind(server_addr);
    server_socket->listen(128);
    std::cout << "Server started." << std::endl;

    // epoll
    EventLoop loop;
    std::shared_ptr<Channel> server_channel = std::make_shared<Channel>(loop.ep(), listened_fd);
    server_channel->enablereading();
    server_channel->set_read_callback(std::bind(&Channel::new_connection, server_channel, server_socket));
    loop.run();
    return 0;
}