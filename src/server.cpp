
#include "tcpServer.h"

int main()
{
    TcpServer tcpServer("127.0.0.1", 7175);
    tcpServer.start();

    return 0;
}