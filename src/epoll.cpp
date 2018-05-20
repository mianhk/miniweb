//
// Created by yu on 2018/5/18.
//

#include "epoll.h"

struct epoll_event* events;
//创建
int mi_epoll_create(int flags){
    int epoll_fd=epoll_create1(flags);
    if(epoll_fd==-1)
        return -1;
    events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * MAXEVENTS);
    return epoll_fd;
}

int mi_epoll_add(int epoll_fd,int listenfd,http_request_t* request,int event){
    struct  epoll_event event;
    event.data.ptr=(void*)request;
    event.events=event;
    int ret=epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
    if(ret==-1)
        return -1;
    return 0;
}