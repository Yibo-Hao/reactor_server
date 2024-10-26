
#include "tcpServer.h"

int main()
{
    // tcpServer 负责创建 server_socket，eventLoop，eventLoop 创建 epoll
    // channel 将 server_socket 和 epoll 关联起来
    // epoll 负责监听 server_socket 的事件，生产新的 socket 并将其加入 epoll
    TcpServer tcpServer("", 7175);
    tcpServer.start();

    return 0;
}