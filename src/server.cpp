#include <csignal>

#include "echoServer.h"

EchoServer *echoServer;

void Stop(int sig) // 信号2和15的处理函数，功能是停止服务程序。
{
    printf("sig=%d\n",sig);
//    echoServer->Stop();
    printf("echoServer已停止。\n");
    delete echoServer;
    printf("delete echoServer。\n");
    exit(0);
}

int main()
{
    signal(SIGTERM, Stop);
    signal(SIGINT, Stop);
    echoServer = new EchoServer("127.0.0.1", 7175, 3, 3);
    echoServer->start();
    return 0;
}