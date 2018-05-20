#include <stdlib.h>
#include <stdio.h>
#include <iostream> 
#include <vector>
#include "util.h"
#include "epoll.h"
#include "http_request.h"
using namespace std;

static int port=3000;
static int thread_num=10;
static char* root="./www";
int main(int argc,char ** argv){
    int listenfd;
    if(argc!=2){
        perror("using: ./miniweb port\n");
        return -1;
    }
    //设置端口
    port=atoi(argv[1]);
    if((port<=1024)||(port>=65536)){
        printf("warning: port out of range.Change to %d\n",5000);
    }
    // 处理SIGPIPE
    handle_for_sigpipe();

    //初始化socket开始监听
    listenfd=socket_bind_listen(port);
    printf("listining fd :%d on port :%d\n",listenfd,port);

    //设置socket为非阻塞
    int rc=set_socket_block(listenfd);

    // 创建epoll并注册监听描述符
    int epoll_fd = mi_epoll_create(0);
    http_request_t* request=(http_request_t*)malloc(sizeof(http_request_t));
    //初始化request
    http_init_request_t(request,listenfd,epoll_fd,root);
    //添加事件
    mi_epoll_add(epoll_fd,listenfd,request,(EPOLLIN|EPOLLET));  //设置epoll触发方式

    //初始化线程池
    threadpool_t *tp=threadpoll_init(thread_num);

    while(1){
        // 得到最近且未删除时间和当前时间差值（等待时间）
        int time = tk_find_timer();

        // 调用epoll_wait函数，返回接收到事件的数量
        int events_num = tk_epoll_wait(epoll_fd, events, MAXEVENTS, -1);

        // 处理已经超时的请求
        tk_handle_expire_timers();

        // 遍历events数组，根据监听种类及描述符类型分发操作
        tk_handle_events(epoll_fd, listen_fd, events, events_num, conf.root, tp);
    }

    // 回收线程资源
    // threadpool_destroy(tp, graceful_shutdown);



    return 0;
}

