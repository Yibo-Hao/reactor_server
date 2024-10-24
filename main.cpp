#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <unistd.h>

void set_nonblocking(int);

int PORT = 7175;

int main() {
    int listened_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listened_fd < 0)
    {
        std::cerr << "socket() failed" << std::endl;
        return -1;
    }

    int opt = 1;
    setsockopt(listened_fd, SOL_SOCKET, SO_REUSEADDR, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, SOL_SOCKET, TCP_NODELAY, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, SOL_SOCKET, SO_REUSEPORT, &opt, static_cast<socklen_t>(sizeof opt));
    setsockopt(listened_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, static_cast<socklen_t>(sizeof opt));

    set_nonblocking(listened_fd);

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(listened_fd, reinterpret_cast<struct sockaddr *>(&server_addr), sizeof(server_addr)) < 0 )
    {
        std::cerr << "bind() failed" << std::endl;
        close(listened_fd);
        return -1;
    }

    if (listen(listened_fd,128) != 0 )
    {
        std::cerr << "listen() failed" << std::endl;
        close(listened_fd);
        return -1;
    }

    return 0;
}

// 设置非阻塞的IO。
void set_nonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}