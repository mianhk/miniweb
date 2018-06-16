#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
const int MAX_BUF = 4096; //缓冲区大小
const int LISTENQ = 2048; //监听队列的长度

int socket_bind_listen(int port)
{
    //检查port
    if (port < 0 || port > 65536)
        return -1;

    //创建socket
    int listen_fd;
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    //消除bind时地址已经被用
    int optval = -1;
    if ((setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1))
        return -1;

    //绑定服务器ip和端口
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    // std::cout << server_addr << std::endl;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    //绑定端口
    if ((bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == -1)
        return -1;

    //监听端口
    if ((listen(listen_fd, LISTENQ)) == -1)
        return -1;

    //处理无效的监听描述符
    if (listen_fd == -1)
    {
        close(listen_fd);
        return -1;
    }
    return listen_fd;
}

int socket_accept(int listen_fd)
{
    int confd;
    char buf[MAX_BUF];
    int n;
    while (1)
    {
        if ((confd = accept(listen_fd, (struct sockaddr *)NULL, NULL)) == -1)
        {
            std::cout << "accept error." << std::endl;
            continue;
        }
        n = recv(confd, buf, MAX_BUF, 0);
        buf[n] = '\0';
        printf("recv from client %s\n", buf);
        close(confd);
    }
    close(listen_fd);
    return 0;
}

void handle_sigpipe()
{
    struct sigation sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = FIG_IGN;
    sa.sa_flags = 0;
    if (sigation(SIGPIPE, &sa, NULL))
        return;
}

int set_socket_nonblocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
        return -1;
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1)
        return -1;
    return 0;
}

void handle_expired_event()
{
    MutexLockGuard lock;
    while (!myTimerQueue.empty())
    {
        shared_ptr<mytimer> ptimer_now = myTimerQueue.top();
        if (ptimer_now->isDeleted())
        {
            myTimerQueue.pop();
            //delete ptimer_now;
        }
        else if (ptimer_now->isvalid() == false)
        {
            myTimerQueue.pop();
            //delete ptimer_now;
        }
        else
        {
            break;
        }
    }
}
