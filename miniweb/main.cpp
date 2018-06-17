#include "util.h"
#include "RequestData.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include <sys/epoll.h>
#include <queue>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <memory>
#include <iostream>
static const int PORT = 5000;
static const int thread_num = 10;

static const int MAXEVENTS = 5000;   //最大事件数目
static const int LISTENQ = 1024;     //监听队列的最大数
const int THREADPOLL_THREAD_NUM = 4; //线程池线程最大数目
const int QUEUE_NUM = 65535;         //队列大小
int main()
{
    handle_sigpipe(); //处理sigpipe信号
    if (Epoll::epoll_init(MAXEVENTS, LISTENQ) < 0)
    {
        perror("epoll init error");
        return -1;
    }

    if (ThreadPool::threadpool_create(THREADPOLL_THREAD_NUM, QUEUE_NUM) < 0)
    {
        perror("ThreadPool create error");
        return -1;
    }

    int listenfd; //监听的文件描述符
    if ((listenfd = socket_bind_listen(PORT)) < 0)
    {
        perror("socket bind error");
        return -1;
    }

    if (set_socket_nonblocking(listenfd) < 0)
    {
        perror("set socket nonblocking error");
        return -1;
    }

    shared_ptr<RequestData> request(new RequestData());
    request->setfd(listenfd);

    if (Epoll::epoll_add(listenfd, request, EPOLLIN | EPOLLET) < 0)
    {
        perror("epoll add error");
        return -1;
    }
    while (true)
    {
        Epoll::epoll_wait1(listenfd, MAXEVENTS, -1); //重载的epoll_wait
        handle_expired_event();
    }

    return 0;
}
