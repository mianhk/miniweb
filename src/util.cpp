#include "util.h"
//map<string, int> config_map;

//int error_output(int error_num,string error_info){
//   printf("error: %s\n",error_info);
//   exit(error_num);
//}
using namespace std;

//信号处理
void handle_for_sigpipe(){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, NULL))
        return;
}

//监听端口
int socket_bind_listen(int port){
    int listenfd;
    struct sockaddr_in server_addr;

    //创建socket
    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("create socket error");
        return -1;
    }

    // 消除bind时"Address already in use"错误
    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int)) == -1){
        return -1;
    }

    //设置服务器IP和port
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=port;
    server_addr.sin_addr.s_addr=INADDR_ANY;

    //绑定
    if((bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))==-1){
        perror("bind ");
        return -1;
    }

    //监听
    if((listen(listenfd,10))==-1){
        perror("listen");
        return -1;
    }
    if(listenfd==-1){
        perror("listenfd");
        return -1;
    }

    return listenfd;
    
}



int set_socket_block(int fd){
    int flag=fcntl(fd,F_GETFD,0);
    if(-1==flag){
        perror("getfd");
        return -1;
    }
    flag|=O_NONBLOCK;
    if(fcntl(fd,F_SETFL,flag)==-1){
        perror("setfl");
        return -1;
    }
}






