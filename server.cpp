#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

int PORT = 7175;

int main() {
    int listened_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (listened_fd < 0)
    {
        std::cerr << "socket() failed" << std::endl;
        return -1;
    }

    int opt = 1;
    setsockopt(listened_fd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, IPPROTO_TCP, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));

    struct sockaddr_in server_addr{};
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(listened_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0)
    {
        std::cerr << "bind() failed" << std::endl;
        close(listened_fd);
        return -1;
    }

    if (listen(listened_fd, 128) != 0)
    {
        std::cerr << "listen() failed" << std::endl;
        close(listened_fd);
        return -1;
    }

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
                    struct sockaddr_in client_addr{};
                    socklen_t len = sizeof(client_addr);
                    int client_fd = accept4(listened_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &len, SOCK_NONBLOCK);
                    std::cout << "New client connected: " << client_fd << inet_ntoa(client_addr.sin_addr) << ntohs(client_addr.sin_port) << std::endl;
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