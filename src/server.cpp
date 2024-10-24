#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <memory>

#include "inet_address.h"
#include "socket.h"
#include "epoll.h"

int PORT = 7175;

int main() {
    std::unique_ptr<Socket> server_socket = std::make_unique<Socket>(create_nonblocking());
    InetAddress server_addr(INADDR_ANY, PORT);

    int listened_fd = server_socket->fd();
    server_socket->set_reuse_addr(true);
    server_socket->set_reuse_port(true);
    server_socket->set_tcp_no_delay(true);
    server_socket->set_keepalive(true);
    server_socket->bind(server_addr);
    server_socket->listen(128);
    std::cout << "Server started." << std::endl;

    Epoll epoll;
    epoll.add_fd(listened_fd, EPOLLIN);
    std::vector<epoll_event> evs;

    while(true)
    {
        evs = epoll.loop();
        for (auto &ev : evs) {
            if (ev.events & EPOLLRDHUP)
            {
                close(ev.data.fd);
                std::cout << "Client disconnected: " << ev.data.fd << std::endl;
            }
            else if (ev.events & (EPOLLIN|EPOLLPRI))
            {
                if (ev.data.fd == listened_fd)
                {
                    InetAddress client_addr{};
                    int client_fd = server_socket->accept(client_addr);
                    std::cout << "New client connected: " << client_fd << client_addr.ip() << client_addr.port() << std::endl;
                    epoll.add_fd(client_fd, EPOLLIN | EPOLLET);
                }
                else {
                    char buffer[1024];
                    while (true)
                    {
                        memset(&buffer, 0, sizeof(buffer));
                        ssize_t n_read = recv(ev.data.fd, buffer, sizeof(buffer), 0);
                        if (n_read > 0)
                        {
                            std::cout << "recv: " << buffer << std::endl;
                            send(ev.data.fd, "OK", 2, 0);
                        }
                        else if (n_read == -1 && errno == EINTR) // 读取数据的时候被信号中断，继续读取。
                        {
                            continue;
                        }
                        else if (n_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) // 全部的数据已读取完毕。
                        {
                            break;
                        }
                        else if (n_read == 0)
                        {
                            std::cout << "Client disconnected: " << ev.data.fd << std::endl;
                            close(ev.data.fd);
                            break;
                        }
                    }
                }
            }
            else
            {
                std::cout << "Client error: " << ev.data.fd << std::endl;
                close(ev.data.fd);
            }
        }
    }

    return 0;
}