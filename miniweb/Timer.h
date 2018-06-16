#include "RequestData.h"
class Timer
{
public:
  bool deleted;
  size_t expired_time;
  std::shared_ptr<RequestData> request_data;

  Timer(std::shared_ptr<RequestData> _request_data, int _timeout);
  ~mytimer();
  void update(int timeout);
  vool is_valid();
  void clear_req();
  void set_deleted();
  bool is_deleted();
  size_t get_expire_time();
}
