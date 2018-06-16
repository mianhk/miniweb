#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <iostream>
#include "../Log.h"
#define SERVER_ADDR "127.0.0.1"
const int PORT = 4000;
// const std::string SERVER_ADDR = "127.0.0.1";

const int MAX_BUF = 4096;

int setSocketNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
        return -1;

    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1)
        return -1;

    return 0;
}
int main()
{
    LOG i("jdskajdla");
    int sockfd;
    struct sockaddr_in server_addr;
    char sendline[MAX_BUF];
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        return -1;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    // server_addr.sin_addr.s_addr = htonl(SERVER_ADDR);
    inet_pton(AF_INET, SERVER_ADDR, &(server_addr.sin_addr));

    char buf[4096];
    buf[0] = '\0';
    //发空串
    const char *p = " ";

    if ((connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) == 0)
    {
        /* setSocketNonBlocking(sockfd);
        cout<<"1: "<<endl;
        //ssize_t 就是有符号的size_t
        ssize_t n=write(sockfd,p,strlen(p));
        cout<<"strlen(p)= "<<strlen(p)<<endl;
        sleep(1);
        n=read(sockfd,buff,MAX_BUF); */
        std::cout << "send message to server:" << std::endl;
        fgets(sendline, MAX_BUF, stdin);
        if (send(sockfd, sendline, strlen(sendline), 0) < 0)
        {
            std::cout << "send message error. " << std::endl;
            return 0;
        }

        close(sockfd);
    }
    std::cout << "----------------------" << std::endl;
    return 0;
}
