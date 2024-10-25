#include <iostream>
#include <netinet/in.h>
#include <memory>
#include <algorithm>

#include "inetAddress.h"
#include "socket.h"
#include "epoll.h"
#include "channel.h"

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
    Epoll epoll;
    std::shared_ptr<Channel> server_channel = std::make_shared<Channel>(&epoll, listened_fd, true);
    server_channel->enablereading();

    while(true)
    {
        std::vector<Channel*> channels = epoll.loop();
        std::for_each(channels.begin(), channels.end(), [&server_socket](Channel* channel) {
            channel->handle_event(server_socket);
        });
    }

    return 0;
}