#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Timer.h"

const int STATE_PARSE_URI = 1;
const int STATE_PARSE_HEADERS = 2;
const int STATE_RECV_BODY = 3;
const int STATE_ANALYSIS = 4;
const int STATE_FINISH = 5;

const int MAX_BUFF = 4096;

// 有请求出现但是读不到数据,可能是Request Aborted,
// 或者来自网络的数据没有达到等原因,
// 对这样的请求尝试超过一定的次数就抛弃
const int AGAIN_MAX_TIMES = 200;

const int PARSE_URI_AGAIN = -1;
const int PARSE_URI_ERROR = -2;
const int PARSE_URI_SUCCESS = 0;

const int PARSE_HEADER_AGAIN = -1;
const int PARSE_HEADER_ERROR = -2;
const int PARSE_HEADER_SUCCESS = 0;

const int ANALYSIS_ERROR = -2;
const int ANALYSIS_SUCCESS = 0;

const int METHOD_POST = 1;
const int METHOD_GET = 2;
const int HTTP_10 = 1;
const int HTTP_11 = 2;

const int EPOLL_WAIT_TIME = 500;

class MimeType
{
private:
  static pthread_mutex_t lock;
  static std::unordered_map<std::string, std::string> mime;
  MimeType();
  MimeType(const MimeType &m);

public:
  static std::string getMime(const std::string &suffix);
};

enum HeadersState
{
  h_start = 0,
  h_key,
  h_colon,
  h_spaces_after_colon,
  h_value,
  h_CR,
  h_LF,
  h_end_CR,
  h_end_LF
};
class Timer;
class RequestData;

class RequestData : public std::enable_shared_from_this<RequestData>
{
private:
  int again_times;  //重复时间
  std::string path; //
  int fd;
  int epollfd;
  std::string content;   //用完就清理的接收请求
  int method;            //http请求类型
  int HTTPv;             //http版本
  std::string file_name; //文件名
  int read_pos;          //读取指针位置
  int state;
  int h_state;
  bool is_finish;
  bool keep_alive;
  std::unordered_map<std::string, std::string> headers;
  std::weak_ptr<Timer> timer;

  int parse_URI();
  int parse_headers();
  int parse_request();

public:
  RequestData();
  RequestData(int _epollfd, int _fd, std ::string _path);
  ~RequestData();
  void add_timer(std::shared_ptr<Timer> mtimer);
  void reset();
  void seperate_timer();
  int getfd();
  void setfd(int _fd);
  void handle_request();
  void handle_error(int fd, int err_num, std::string msg);
};

class MutexLockGuard
{
public:
  explicit MutexLockGuard();
  ~MutexLockGuard();

private:
  static pthread_mutex_t lock;

private:
  MutexLockGuard(const MutexLockGuard &);
  MutexLockGuard &operator=(const MutexLockGuard &);
};
