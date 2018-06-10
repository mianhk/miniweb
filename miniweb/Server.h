#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>

class Server
{
  public:
    Server(EventLoop *loop, int thread_num, int port);
    ~Server() {}
    EventLoop *getLoop() const { return _loop; }
    void start();
    void handNewConn();
    void handThisConn() { _loop->updatePooler(_accept_channel); }

  private:
    EventLoop *_loop;
    int _thread_num;
    std::unique_ptr<EventLoopThreadPool> _event_loop_thread_poll;
    bool _started;
    std::shared_ptr(Channel) _accept_channel;
    int _port;
    int _listenfd;
    static const int MAXFDS = 100000;
}
