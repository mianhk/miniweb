#pragma once
#include "RequestData.h"
#include <string>
#include <unordered_map>
#include <memory>

class RequestData;
class Timer
{
public:
  bool deleted;
  size_t expired_time;
  std::shared_ptr<RequestData> request_data;

  Timer(std::shared_ptr<RequestData> _request_data, int _timeout);
  ~Timer();
  void update(int timeout);
  bool is_valid();
  void clear_req();
  void set_deleted();
  bool is_deleted() const;
  size_t get_expire_time() const;
};

struct timerCmp
{
  bool operator()(std::shared_ptr<Timer> &a, std::shared_ptr<Timer> &b) const;
};
