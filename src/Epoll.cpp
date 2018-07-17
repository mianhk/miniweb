#include "Epoll.h"
#include "ThreadPool.h"
#include "util.h"
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <queue>
#include <deque>

int TIMER_TIME_OUT = 500;
extern std::priority_queue<std::shared_ptr<Timer>, std::deque<std::shared_ptr<Timer>>, timerCmp> TimerQueue;

epoll_event *Epoll::events;
std::unordered_map<int, std::shared_ptr<RequestData>> Epoll::fd2req;
int Epoll::epoll_fd = 0;
std::string Epoll::PATH = "./";

int Epoll::epoll_init(int max_events, int listen_num, std::string path)
{
    PATH = path;
    std::cout << "PATH" << PATH << std::endl;
    if ((epoll_fd = epoll_create1(EPOLL_CLOEXEC)) == -1)
        return -1; //它是fd的一个标识说明，用来设置文件close-on-exec状态的。当close-on-exec状态为0时，调用exec时，fd不会被关闭；状态非零时则会被关闭，这样做可以防止fd泄露给执行exec后的进程。关于exec的用法，大家可以去自己查阅下，或者直接man exec。
    events = new epoll_event[max_events];
    return 0;
}

// 注册新描述符
int Epoll::epoll_add(int fd, std::shared_ptr<RequestData> request, __uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        return -1;
    }
    fd2req[fd] = request;
    return 0;
}

// 修改描述符状态
int Epoll::epoll_mod(int fd, std::shared_ptr<RequestData> request, __uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        perror("epoll_mod error");
        return -1;
    }
    fd2req[fd] = request;
    return 0;
}

// 从epoll中删除描述符
int Epoll::epoll_del(int fd, __uint32_t events)
{
    struct epoll_event event;
    event.data.fd = fd;
    event.events = events;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
        return -1;
    }
    auto fd_ite = fd2req.find(fd);
    if (fd_ite != fd2req.end())
        fd2req.erase(fd_ite);
    return 0;
}

// 返回活跃事件数
void Epoll::epoll_wait1(int listen_fd, int max_events, int timeout)
{
    // printf("fd2req.size()==%d\n", fd2req.size());
    int event_count = epoll_wait(epoll_fd, events, max_events, timeout);
    if (event_count < 0)
        perror("epoll wait error");
    std::vector<std::shared_ptr<RequestData>> req_data = get_events_request(listen_fd, event_count, PATH);
    if (req_data.size() > 0)
    {
        for (auto &req : req_data)
        {
            if (ThreadPool::threadpool_add(req) < 0)
            {
                // 线程池满了或者关闭了等原因，抛弃本次监听到的请求。
                break;
            }
        }
    }
}
#include <iostream>
#include <arpa/inet.h>
using namespace std;
void Epoll::accept_connection(int listen_fd, int epoll_fd, const std::string path)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = 0;
    int accept_fd = 0;
    while ((accept_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len)) > 0)
    {

        // cout << inet_addr(client_addr.sin_addr.s_addr) << endl;
        // cout << client_addr.sin_port << endl;
        /*
        // TCP的保活机制默认是关闭的
        int optval = 0;
        socklen_t len_optval = 4;
        getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        cout << "optval ==" << optval << endl;
        */

        // 设为非阻塞模式
        int ret = set_socket_nonblocking(accept_fd);
        if (ret < 0)
        {
            perror("Set non block failed!");
            return;
        }

        std::shared_ptr<RequestData> req_info(new RequestData(epoll_fd, accept_fd, path));

        // 文件描述符可以读，边缘触发(Edge Triggered)模式，保证一个socket连接在任一时刻只被一个线程处理
        __uint32_t _epo_event = EPOLLIN | EPOLLET | EPOLLONESHOT;
        Epoll::epoll_add(accept_fd, req_info, _epo_event);
        // 新增时间信息
        std::shared_ptr<Timer> mtimer(new Timer(req_info, TIMER_TIME_OUT));
        req_info->add_timer(mtimer);
        MutexLockGuard lock;
        TimerQueue.push(mtimer);
    }
    //if(accept_fd == -1)
    //   perror("accept");
}

// 分发处理函数
std::vector<std::shared_ptr<RequestData>> Epoll::get_events_request(int listen_fd, int events_num, const std::string path)
{
    std::vector<std::shared_ptr<RequestData>> req_data;
    for (int i = 0; i < events_num; ++i)
    {
        // 获取有事件产生的描述符
        int fd = events[i].data.fd;

        // 有事件发生的描述符为监听描述符
        if (fd == listen_fd)
        {
            //cout << "This is listen_fd" << endl;
            accept_connection(listen_fd, epoll_fd, path);
        }
        else if (fd < 3)
        {
            break;
        }
        else
        {
            // 排除错误事件
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
            {
                //printf("error event\n");
                auto fd_ite = fd2req.find(fd);
                if (fd_ite != fd2req.end())
                    fd2req.erase(fd_ite);
                //printf("fd = %d, here\n", fd);
                continue;
            }

            // 将请求任务加入到线程池中
            // 加入线程池之前将Timer和request分离
            std::shared_ptr<RequestData> cur_req(fd2req[fd]);
            //printf("cur_req.use_count=%d\n", cur_req.use_count());
            cur_req->seperate_timer();
            req_data.push_back(cur_req);
            auto fd_ite = fd2req.find(fd);
            if (fd_ite != fd2req.end())
                fd2req.erase(fd_ite);
        }
    }
    return req_data;
}
