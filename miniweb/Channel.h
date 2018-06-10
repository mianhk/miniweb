#include "Timer.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <sys/epoll.h>
#include <functional>
#include <sys/epoll.h>


class EventLoop;
class HttpData;

class Channel{
    private:
        typedef std::function<void()> CallBack;
        EventLoop *_loop;
        int _fd;
        _uint32_t _events;
        _uint32_t _revents;
        _uint32_t _lastevents;
        
        //找到上层持有的Channel对象
        // std::wake_ptr<>
        
        
    public:
        Channel(EventLoop *loop);
        Channel(EventLoop *loop, int fd);
        ~Channel();
        
        int getFd();
        void setFd(int fd);
        
}
