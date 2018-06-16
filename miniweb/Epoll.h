#pragma once
#include "Timer.h"
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <memory>

class Epoll
{
private:
  static epoll_event *events;
  static std::unordered_map<int, std::shared_ptr<RequestData>> fd2req;
  static int epoll_fd;
  static const std::string PATH;

public:
  static int epoll_init(int max_events, int listen_num);
  static int epoll_add(int fd, std::shared_ptr<RequestData> request, __uint32_t events);
  static int epoll_mod(int fd, std::shared_ptr<RequestData>, __uint32_t events);
  static int epoll_del(int fd, __uint32_t events);
  static void epoll_wait1(int listen_fd, int max_events, int timeout);
  static void acceptConnection(int listen_fd, int epoll_fd, const std::string path);
  static std::vector<std::shared_ptr<RequestData>> getEventsRequest(int listen_fd, int events_num, const std::string path);
};
