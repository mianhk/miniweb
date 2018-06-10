#include "Epoll.h"
#include "Channel.h"
#include "Util.h"
#include <vector>
#include <memory>
#include <functional>

#include <iostream>
using namespace std;

class EventLoop
{
private:
  /* data */
  bool _looping;             //循环
  shared_ptr<Epoll> _poller; //
  int _wakeupFd;
  bool _quit;
  bool _enentHandling;
  mutable MutexLock _mutex;
  std::vector<Functor> _pendingFuntors;
  bool _callingPendingFunctors;
  const pid_t _threadId;
  shared_ptr<Channel> _pwakeupChannel;

  void wakeup();
  void handleRead();
  void doPendingFunctors;
  void handleConnect();

public:
  typedef std::function<void()> Functor;
  EventLoop();
  ~EventLoop();
  void loop(); //循环
  void quit();
  void runInLoop(Functor &&cb);
  void queueInLoop(Functor &&cb);
  // bool isInLoopThread() const { return _threadId == }

  // void assertInLoopThread(){
  //   assert()
  // }

  void shutdown(chared_ptr<Channel> channel)
  {
    shutdownWR(channel->getFd());
  }

  void removeFromPoller(shared_ptr<Channel> channel)
  {
    _poller->epoll_del(channel);
  }
  void updatePoller(shared_ptr<Channel> channel, int timeout = 0)
  {
    _poller->epoll_mod(channel, timeout);
  }
  void addToPoller(shared_ptr<Channel> channel,int timeout=0){
    _poller->epoll_add(channel,timeout);
  }
};
