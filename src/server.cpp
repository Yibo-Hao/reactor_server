
#include "echoServer.h"

int main()
{
    EchoServer echoServer("127.0.0.1", 7175, 1, 3);
    echoServer.start();

    return 0;
}