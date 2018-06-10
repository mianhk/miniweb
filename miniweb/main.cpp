#include "util.h"
#include <iostream>
int main()
{
    int threadNum = 0; //threadNum 线程数量
    int port = 4000;
    std::cout << threadNum++ << std::endl;
    int listenfd = 0;
    listenfd = socket_bind_listen(port);
    socket_accept(listenfd);
    std::cout << listenfd << "   *************" << std::endl;

    return 0;
}
