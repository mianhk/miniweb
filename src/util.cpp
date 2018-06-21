#include "util.h"
#include "RequestData.h"
#include "Timer.h"
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <queue>
#include <deque>
#include <fstream>
#include <sstream>
#include <iostream>
const int MAX_BUF = 4096; //缓冲区大小
const int LISTENQ = 2048; //监听队列的长度

extern std::priority_queue<std::shared_ptr<Timer>, std::deque<std::shared_ptr<Timer>>, timerCmp> TimerQueue;

void read_config(int &port, std::string &path, int &thread_num)
{
    std::fstream config("../config");
    std::string line;
    while (config >> line)
    {
        // std::cout << "line: " << line << std::endl;
        int index = line.find("=");
        std::string prefix_line = line.substr(0, index);
        // std::cout << "prefix_line: " << prefix_line << std::endl;
        std::string suffix = line.substr(index + 1);
        std::stringstream ss;
        ss << suffix;
        if (prefix_line == "port")
            ss >> port;
        if (prefix_line == "path")
            path = suffix;
        if (prefix_line == "thread_num")
            ss >> thread_num;
    }
    return;
}
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
    std::cout << "Miniserver linsen on port: " << port << std::endl;
    return listen_fd;
}

// int socket_accept(int listen_fd)
// {
//     int confd;
//     char buf[MAX_BUF];
//     int n;
//     while (1)
//     {
//         if ((confd = accept(listen_fd, (struct sockaddr *)NULL, NULL)) == -1)
//         {
//             std::cout << "accept error." << std::endl;
//             continue;
//         }
//         n = recv(confd, buf, MAX_BUF, 0);
//         buf[n] = '\0';
//         printf("recv from client %s\n", buf);
//         close(confd);
//     }
//     close(listen_fd);
//     return 0;
// }

void handle_sigpipe()
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL))
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
    while (!TimerQueue.empty())
    {
        std::shared_ptr<Timer> ptimer_now = TimerQueue.top();
        if (ptimer_now->is_deleted())
        {
            TimerQueue.pop();
            //delete ptimer_now;
        }
        else if (ptimer_now->is_valid() == false)
        {
            TimerQueue.pop();
            //delete ptimer_now;
        }
        else
        {
            break;
        }
    }
}

// void accept_connection(int listen_fd, int epoll_fd, const std::string &path)
// {
//     struct sockaddr_in client_addr;
//     memset(&client_addr, 0, sizeof(client_addr));
//     socklen_t client_addr_len = 0;
//     int accept_fd = 0;
//     while ((accept_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len)) > 0)
//     {
//         //设为非阻塞模式
//         int ret = set_socket_nonblocking(accept_fd);
//         if (ret < 0)
//         {
//             perror("set nonblock failed!");
//             return;
//         }
//     }
// }

ssize_t readn(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nread = 0;
    ssize_t readSum = 0;
    char *ptr = (char *)buff;
    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;
            else if (errno == EAGAIN)
            {
                return readSum;
            }
            else
            {
                return -1;
            }
        }
        else if (nread == 0)
            break;
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n)
{
    size_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char *)buff;
    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    nwritten = 0;
                    continue;
                }
                else
                    return -1;
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}
