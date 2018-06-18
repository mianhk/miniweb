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

// The MIT License (MIT)

// Copyright (c) 2018 mianhk

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

static const int MAXEVENTS = 5000; //最大事件数目
static const int LISTENQ = 1024;   //监听队列的最大数
const int QUEUE_NUM = 65535;       //队列大小
int main()
{
    int port = 3000; //端口地址
    std::string path = "./www";
    int thread_num = 4; //线程池线程最大数目
    // std::cout << "port: " << port << std::endl;
    read_config(port, path, thread_num);
    // std::cout << "port: " << port << std::endl;
    handle_sigpipe(); //处理sigpipe信号
    if (Epoll::epoll_init(MAXEVENTS, LISTENQ, path) < 0)
    {
        perror("epoll init error");
        return -1;
    }

    if (ThreadPool::threadpool_create(thread_num, QUEUE_NUM) < 0)
    {
        perror("ThreadPool create error");
        return -1;
    }

    int listenfd; //监听的文件描述符
    if ((listenfd = socket_bind_listen(port)) < 0)
    {
        perror("socket bind error");
        return -1;
    }

    if (set_socket_nonblocking(listenfd) < 0)
    {
        perror("set socket nonblocking error");
        return -1;
    }

    shared_ptr<RequestData> request(new RequestData(path));
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
