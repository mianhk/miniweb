#pragma once
#include <iostream>
#include <string>

const string path = ".";
const string name = "log.txt";
class LOG
{
  friend public : LOG(string s) : log_path(path), log_name(name), s(s)
  {
  }
  ~LOG() {}

  string log_path; //log目录
  string log_name; //log文件名
  string s;        //输入到文件中的字符串
private:
  void write_log(LOG log);
};

#define log LOG(s)
