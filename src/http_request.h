<<<<<<< HEAD
//
// Created by yu on 2018/5/18.
//

#ifndef SRC_HTTP_REQUEST_H
#define SRC_HTTP_REQUEST_H

#include "list.h"
#define MAX_BUF 8012

typedef struct http_request{
    char* root;
    int fd;
    int epoll_fd;
    char buff[MAX_BUF];
    size_t pos;
    size_t last;
    int state;

    void* request_start;
    void* method_end;
    int method;
    void* uri_start;
    void* uri_end;
    void* path_start;
    void* path_end;
    void* query_start;
    void* query_end;
    int http_major;
    int http_minor;
    void* request_end;

    struct list_head list;    // 存储请求头，list.h中定义了此结构

    void* cur_header_key_start;
    void* cur_header_key_end;
    void* cur_header_value_start;
    void* cur_header_value_end;
    void* timer;
}http_request_t;

int http_init_request_t(http_request_t* request,int fd,int epoll_fd,char* path);
int mi_epoll_add(int epoll_fd,int listenfd,http_request_t* request,int event);

#endif //SRC_HTTP_REQUEST_H
=======
//
// Created by yu on 2018/5/18.
//

#ifndef SRC_HTTP_REQUEST_H
#define SRC_HTTP_REQUEST_H

#include "list.h"
#define MAX_BUF 8012

typedef struct http_request{
    char* root;
    int fd;
    int epoll_fd;
    char buff[MAX_BUF];
    size_t pos;
    size_t last;
    int state;

    void* request_start;
    void* method_end;
    int method;
    void* uri_start;
    void* uri_end;
    void* path_start;
    void* path_end;
    void* query_start;
    void* query_end;
    int http_major;
    int http_minor;
    void* request_end;

    struct list_head list;    // 存储请求头，list.h中定义了此结构

    void* cur_header_key_start;
    void* cur_header_key_end;
    void* cur_header_value_start;
    void* cur_header_value_end;
    void* timer;
}http_request_t;

int http_init_request_t(http_request_t* request,int fd,int epoll_fd,char* path);
int mi_epoll_add(int epoll_fd,int listenfd,http_request_t* request,int event);

#endif //SRC_HTTP_REQUEST_H
>>>>>>> 1245ba3f16ae99ced5c12db10ef08baba47f04dc
