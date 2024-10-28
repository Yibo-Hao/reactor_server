#include <unistd.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>

int PORT = 7175;
char IP[] = "127.0.0.1";

int main(int argc, char *argv[])
{
    int socket_fd;
    struct sockaddr_in server_addr{};

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cout << "socket() failed." << std::endl;
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(socket_fd, (struct sockaddr*)(&server_addr), sizeof(server_addr)) != 0)
    {
        std::cout << "connect(" << IP << ":" << PORT << ") failed." << std::endl;
        close(socket_fd);
        return -1;
    }

    std::cout << "connect ok." << std::endl;

    char buf[1024];
    char tempBuf[1024];
    for (int i = 0; i < 200000; i++)
    {
        // 从命令行输入内容。
        memset(buf, 0, sizeof(buf));
        std::cout << "please input: ";
        std::cin >> buf;
        int len = strlen(buf);
        memcpy(tempBuf, &len, 4);
        memcpy(tempBuf + 4, buf, len);

        if (send(socket_fd, tempBuf, len + 4,0) <= 0)       // 把命令行输入的内容发送给服务端。
        {
            std::cout << "write() failed." << std::endl;
            close(socket_fd);
            return -1;
        }

        memset(buf, 0, sizeof(buf));
        if (recv(socket_fd,buf,sizeof(buf),0) <=0)      // 接收服务端的回应。
        {
            std::cout << "read() failed." << std::endl;
            close(socket_fd);
            return -1;
        }
        int recvLen;
        memcpy(&recvLen, buf, 4);
        std::string message(buf + 4, recvLen);

        std::cout << "recv:" << message << std::endl;
    }
}