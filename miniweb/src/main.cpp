#include <stdlib.h>
#include <stdio.h>
#include <iostream> 
#include <vector>
#include "util.h"
using namespace std;
static int port=3000;
int main(int argc,char ** argv){
    int listenfd;
    if(argc!=2){
        perror("using: ./miniweb port\n");
        return -1;
    }
    cout<<argv[1]<<endl;
    port=atoi(argv[1]);
    listenfd=socket_bind_listen(port);
    printf("listining fd :%d on port :%d\n",listenfd,port);
    //设置socket为非阻塞
    int rc=set_socket_block(listenfd);
    //read_config("../config");
    string s;

    cout<<"hello"<<endl;
    return 0;
}

