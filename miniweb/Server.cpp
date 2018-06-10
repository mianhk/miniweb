#include "Server.h"
#include "Util.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

Server::Server(EventLoop *loop, int thread_num, int port)
    : _loop(loop),
      _thread_num(thread_num),
      _event_loop_thread_pool(new EventLoopThreadPool(_loop, thread_num)),
      _started(false),
      _accept_channel(new Channel(_loop)),
      _port(port),
      _listenfd(socket_bind_listen(_port)),
{
    _accept_channel->setFd(_listenfd);
    handle_for_sigpipe();
}
