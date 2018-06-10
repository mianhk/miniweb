
#include <cstdlib>
#include <string>

int socket_bind_listen(int port);
int socket_accept(int listen_fd);
void handle_for_sigpipe();
int set_socket_nonblocking(int fd);

