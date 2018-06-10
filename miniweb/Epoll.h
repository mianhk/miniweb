#include "Channel.h"
#include "HttpData.h"
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>
class Epoll
{
private:
  /* data */
public:
  Epoll(/* args */);
  ~Epoll();
  void epoll_add(SP_Channel request, int timeout);
  void epoll_mod(SP_Channel request, int timeout);
  void epoll_del(SP_Channel request);

  std::vector<std::shared_ptr<Channel>> poll();
  std::vector<std::shared_ptr<Channel>> getEventsRequest(int events_num);

  void add_timer(std::shared_ptr<Channel> request_data, int timeout);

  int getEpollFd() { return _epollFd; }
  void handleExpired();

private:
  static const int MAXFDS = 100000;
  int _epollFd;
  std::vector<epoll_event> _events;
  std::shared_ptr<Channel> _fd2chan[MAXFDS];
  // std::shared_ptr<> _fd2chan[MAXFDS];
  // Timer
};
