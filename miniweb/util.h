#pragma once
#include <cstdlib>
#include <string>
#include <iostream>
int socket_bind_listen(int port);
int socket_accept(int listen_fd);
int set_socket_nonblocking(int fd);
void handle_sigpipe();
void handle_expired_event();
void accept_connection(int listen_fd, int epoll_fd, const std::string &path);

ssize_t readn(int fd, void *buff, size_t n);
ssize_t writen(int fd, void *buff, size_t n);
