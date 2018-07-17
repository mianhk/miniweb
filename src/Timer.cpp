#include "Timer.h"
#include "util.h"
#include "Epoll.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <queue>
#include <cstdlib>
#include <string.h>

#include <iostream>
using namespace std;

Timer::Timer(shared_ptr<RequestData> _request_data, int timeout) : deleted(false),
                                                                   request_data(_request_data)
{
    // cout << "Timer()" << endl;
    struct timeval now;
    gettimeofday(&now, NULL);
    // cout << "now tv_sec: " << now.tv_sec << endl;
    // 以毫秒计
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

Timer::~Timer()
{
    cout << "~Timer()" << endl;
    if (request_data)
    {
        Epoll::epoll_del(request_data->getfd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
    }
    //request_data.reset();
    // if (request_data)
    // {
    //     cout << "request_data=" << request_data << endl;
    //     delete request_data;
    //     request_data = NULL;
    // }
}

void Timer::update(int timeout)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    expired_time = ((now.tv_sec * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool Timer::is_valid()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = ((now.tv_sec * 1000) + (now.tv_usec / 1000));
    if (temp < expired_time)
    {
        return true;
    }
    else
    {
        this->set_deleted();
        return false;
    }
}

void Timer::clear_req()
{
    request_data.reset();
    this->set_deleted();
}

void Timer::set_deleted()
{
    deleted = true;
}

bool Timer::is_deleted() const
{
    return deleted;
}

size_t Timer::get_expire_time() const
{
    return expired_time;
}

bool timerCmp::operator()(shared_ptr<Timer> &a, shared_ptr<Timer> &b) const
{
    return a->get_expire_time() > b->get_expire_time();
}
