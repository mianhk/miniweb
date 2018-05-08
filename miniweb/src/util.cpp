#include "util.h"
//map<string, int> config_map;

//int error_output(int error_num,string error_info){
//   printf("error: %s\n",error_info);
//   exit(error_num);
//}
using namespace std;

int socket_bind_listen(int port){
    int listenfd;
    struct sockaddr_in server_addr;

    if((listenfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("create socket error");
        return -1;
    }

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










/*
int read_config(const char* path){
   fstream config_file(path,fstream::in);
   string line,word;
   if(!config_file){
        printf("can't open %s\n",path);
        config_file.close();
        return(CONFIG_ERROR);
   }
   while(getline(config_file,line)){
       if("end"==line){
           cout<<"read end"<<endl;
           return 0;
       }
       reutrn 0;
       //初始化config_map
       config_map.insert("port",INIT_PORT);
        //temp=line.split("=");
   }
   /*while(getline(config_file,line)){
        stringstream stream(line);
        stream>>word;
        string s;
        if("end"==line){
            printf("文件读取结束\n");
            exit;
        }

        printf("%s\n",word);
   }*/
//}



