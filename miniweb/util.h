
#include <cstdlib>
#include <string>

int socket_bind_listen(int port);
int socket_accept(int listen_fd);
int set_socket_nonblocking(int fd);
void handle_sigpipe();
void handle_expired_event();

void accept_connection(int listen_fd, int epoll_fd, const string &path)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_addr_len = 0;
    int accept_fd = 0;
    while (accept_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len) > 0)
    {
        //设为非阻塞模式
        int ret = set_socket_nonblocking(accept_fd);
        if (ret < 0)
        {
            perror("set nonblock failed!");
            return;
        }
    }
}
