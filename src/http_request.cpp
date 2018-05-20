//
// Created by yu on 2018/5/18.
//

#include "http_request.h"


// 初始化请求数据结构
int http_init_request_t(http_request_t* request,int fd,int epoll_fd,char* path){
    request->fd=fd;
    request->epoll_fd=epoll_fd;
    request->root=path;

    request->pos = 0;
    request->last = 0;
    request->state = 0;

    INIT_LIST_HEAD(&(request->list));
    return 0;
}