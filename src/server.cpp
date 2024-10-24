#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "inet_address.h"
#include "socket.h"

int PORT = 7175;

int main() {
    Socket server_socket{create_nonblocking()};
    int listened_fd = server_socket.fd();
    server_socket.set_reuse_addr(true);
    server_socket.set_reuse_port(true);
    server_socket.set_tcp_no_delay(true);
    server_socket.set_keepalive(true);

    InetAddress server_addr(INADDR_ANY, PORT);
    server_socket.bind(server_addr);
    server_socket.listen(128);
    std::cout << "Server started." << std::endl;

    int epoll_fd = epoll_create(1);
    struct epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listened_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listened_fd, &ev);
    epoll_event evs[10];

    while(true)
    {
        int in_fds = epoll_wait(epoll_fd, evs, 10, -1);

        if (in_fds < 0)
        {
            std::cerr << "epoll_wait() failed" << std::endl;
            break;
        }

        if (in_fds == 0)
        {
            std::cout << "epoll_wait() timeout." << std::endl;
            continue;
        }

        for (int i = 0; i < in_fds; i++) {
            if (evs[i].events & EPOLLRDHUP)
            {
                close(evs[i].data.fd);
                std::cout << "Client disconnected: " << evs[i].data.fd << std::endl;
            }
            else if (evs[i].events & (EPOLLIN|EPOLLPRI))
            {
                if (evs[i].data.fd == listened_fd)
                {
                    InetAddress client_addr{};
                    int client_fd = server_socket.accept(client_addr);
                    std::cout << "New client connected: " << client_fd << client_addr.ip() << client_addr.port() << std::endl;
                    struct epoll_event client_ev{};
                    client_ev.events = EPOLLIN | EPOLLET;
                    client_ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_ev);
                }
                else {
                    char buffer[1024];
                    while (true)
                    {
                        memset(&buffer, 0, sizeof(buffer));
                        ssize_t n_read = recv(evs[i].data.fd, buffer, sizeof(buffer), 0);
                        if (n_read > 0)
                        {
                            std::cout << "recv: " << buffer << std::endl;
                            send(evs[i].data.fd, "OK", 2, 0);
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
                            std::cout << "Client disconnected: " << evs[i].data.fd << std::endl;
                            close(evs[i].data.fd);
                            break;
                        }
                    }
                }
            }
            else
            {
                std::cout << "Client error: " << evs[i].data.fd << std::endl;
                close(evs[i].data.fd);
            }
        }
    }

    return 0;
}