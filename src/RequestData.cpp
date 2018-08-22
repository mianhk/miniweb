#include "RequestData.h"
#include "util.h"
#include "Epoll.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/time.h>
#include <unordered_map>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <queue>
#include <cstdlib>
#include <string.h>

//test
#include <iostream>
using namespace std;

pthread_mutex_t MutexLockGuard::lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t MimeType::lock = PTHREAD_MUTEX_INITIALIZER;
std::unordered_map<std::string, std::string> MimeType::mime;

std::string MimeType::getMime(const std::string &suffix)
{
    if (mime.size() == 0)
    {
        pthread_mutex_lock(&lock);
        if (mime.size() == 0)
        {
            mime[".html"] = "text/html";
            mime[".avi"] = "video/x-msvideo";
            mime[".bmp"] = "image/bmp";
            mime[".c"] = "text/plain";
            mime[".doc"] = "application/msword";
            mime[".gif"] = "image/gif";
            mime[".gz"] = "application/x-gzip";
            mime[".htm"] = "text/html";
            mime[".ico"] = "application/x-ico";
            mime[".jpg"] = "image/jpeg";
            mime[".png"] = "image/png";
            mime[".txt"] = "text/plain";
            mime[".mp3"] = "audio/mp3";
            mime["default"] = "text/html";
        }
        pthread_mutex_unlock(&lock);
    }
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];
}

std::priority_queue<shared_ptr<Timer>, std::deque<shared_ptr<Timer>>, timerCmp> TimerQueue;

RequestData::RequestData() : read_pos(0),
                             state(STATE_PARSE_URI),
                             h_state(h_start),
                             keep_alive(false),
                             again_times(0)
{
    // cout << "RequestData()" << endl;
}

RequestData::RequestData(std::string _path) : read_pos(0),
                                              state(STATE_PARSE_URI),
                                              h_state(h_start),
                                              keep_alive(false),
                                              again_times(0),
                                              path(_path)
{
}
RequestData::RequestData(int _epollfd, int _fd, std::string _path) : read_pos(0),
                                                                     state(STATE_PARSE_URI),
                                                                     h_state(h_start),
                                                                     keep_alive(false),
                                                                     again_times(0),
                                                                     path(_path),
                                                                     fd(_fd),
                                                                     epollfd(_epollfd)
{
    // cout << "RequestData() with para" << endl;
}

RequestData::~RequestData()
{
    // cout << "~RequestData()" << endl;
    close(fd);
}

void RequestData::add_timer(shared_ptr<Timer> mtimer)
{
    timer = mtimer;
}

int RequestData::getfd()
{
    return fd;
}
void RequestData::setfd(int _fd)
{
    fd = _fd;
}

void RequestData::reset()
{
    again_times = 0;
    content.clear();
    file_name.clear();
    path.clear();
    read_pos = 0;
    state = STATE_PARSE_URI;
    h_state = h_start;
    headers.clear();
    keep_alive = false;
    if (timer.lock())
    {
        shared_ptr<Timer> my_timer(timer.lock());
        my_timer->clear_req();
        timer.reset();
    }
    cout << "RequestData reset over." << endl;
}

void RequestData::seperate_timer()
{
    if (timer.lock())
    {
        shared_ptr<Timer> my_timer(timer.lock());
        my_timer->clear_req();
        timer.reset();
    }
}

void RequestData::handle_request()
{
    // cout << "RequestData handle_request begin." << endl;
    char buff[MAX_BUFF];
    bool isError = false;
    while (true)
    {
        int read_num = readn(fd, buff, MAX_BUFF);
        if (read_num < 0)
        {
            perror("1");
            isError = true;
            break;
        }
        else if (read_num == 0)
        {
            // 有请求出现但是读不到数据，可能是Request Aborted，或者来自网络的数据没有达到等原因
            // perror("read_num == 0");
            if (errno == EAGAIN)
            {
                if (again_times > AGAIN_MAX_TIMES)
                    isError = true;
                else
                    ++again_times;
            }
            else if (errno != 0)
                isError = true;
            break;
        }
        string now_read(buff, buff + read_num);
        content += now_read;

        if (state == STATE_PARSE_URI) //默认状态,先解析URI
        {
            // cout << "-----state==STATE_PARSE_URI" << endl;
            int flag = this->parse_URI();
            if (flag == PARSE_URI_AGAIN)
            {
                break;
            }
            else if (flag == PARSE_URI_ERROR)
            {
                perror("2");
                isError = true;
                break;
            }
            state = STATE_PARSE_HEADERS; //解析URI成功，修改state值，接下来解析headers
        }
        if (state == STATE_PARSE_HEADERS)
        {
            int flag = this->parse_headers();
            if (flag == PARSE_HEADER_AGAIN)
            {
                break;
            }
            else if (flag == PARSE_HEADER_ERROR)
            {
                cout << "-----state==PARSE_HEADER_ERROR" << endl;
                perror("3");
                isError = true;
                break;
            }
            if (method == METHOD_POST)
            {
                state = STATE_RECV_BODY;
            }
            else
            {
                state = STATE_ANALYSIS;
            }
        }
        if (state == STATE_RECV_BODY)
        {
            int content_length = -1;
            if (headers.find("Content-length") != headers.end())
            {
                content_length = stoi(headers["Content-length"]);
            }
            else
            {
                isError = true;
                break;
            }
            if (content.size() < content_length)
                continue;
            state = STATE_ANALYSIS;
        }
        if (state == STATE_ANALYSIS)
        {
            int flag = this->parse_request();
            if (flag < 0)
            {
                isError = true;
                break;
            }
            else if (flag == PARSE_REQUEST_SUCCESS)
            {

                state = STATE_FINISH;
                break;
            }
            else
            {
                isError = true;
                break;
            }
        }
    }

    if (isError)
    {
        return;
    }
    // 加入epoll继续
    if (state == STATE_FINISH)
    {
        if (keep_alive)
        {
            //printf("ok\n");
            this->reset();
        }
        else
        {
            return;
        }
    }
    // 一定要先加时间信息，否则可能会出现刚加进去，下个in触发来了，然后分离失败后，又加入队列，最后超时被删，然后正在线程中进行的任务出错，double free错误。
    // 新增时间信息
    shared_ptr<Timer> mtimer(new Timer(shared_from_this(), 200));
    this->add_timer(mtimer);
    {
        MutexLockGuard lock;
        TimerQueue.push(mtimer);
    }

    __uint32_t _epo_event = EPOLLIN | EPOLLET | EPOLLONESHOT;
    int ret = Epoll::epoll_mod(fd, shared_from_this(), _epo_event);
    if (ret < 0)
    {
        // 返回错误处理
        return;
    }
}

int RequestData::parse_URI()
{
    string &str = content;
    cout << "parse_URI   content:" << content << endl;
    // 读到完整的请求行再开始解析请求
    int pos = str.find('\r', read_pos);
    if (pos < 0)
    {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间
    string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();
    // Method
    pos = request_line.find("GET");
    if (pos < 0)
    {
        pos = request_line.find("POST");
        if (pos < 0)
        {
            return PARSE_URI_ERROR;
        }
        else
        {
            method = METHOD_POST;
        }
    }
    else
    {
        method = METHOD_GET;
    }
    // filename
    pos = request_line.find("/", pos);
    if (pos < 0)
    {
        return PARSE_URI_ERROR;
    }
    else
    {
        int _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                file_name = request_line.substr(pos + 1, _pos - pos - 1);
                int __pos = file_name.find('?');
                if (__pos >= 0)
                {
                    file_name = file_name.substr(0, __pos);
                }
            }

            else
                file_name = "index.html";
            // cout << "file_name: " << file_name << endl;
        }
        pos = _pos;
    }
    cout << "file_name: " << file_name << endl;
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
    {
        return PARSE_URI_ERROR;
    }
    else
    {
        if (request_line.size() - pos <= 3)
        {
            return PARSE_URI_ERROR;
        }
        else
        {
            string ver = request_line.substr(pos + 1, 3);
            if (ver == "1.0")
                HTTPv = HTTP_10;
            else if (ver == "1.1")
                HTTPv = HTTP_11;
            else
                return PARSE_URI_ERROR;
            // cout << "HTTP version: " << HTTPv << endl;
        }
    }

    cout << "parse URI success" << endl;
    return PARSE_URI_SUCCESS;
}

int RequestData::parse_headers()
{
    // return PARSE_HEADER_SUCCESS;
    string &str = content;
    cout << "str: " << str << endl;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    for (size_t i = 0; i < str.size() && notFinish; ++i)
    {
        switch (h_state)
        {
        case h_start:
        {
            if (str[i] == '\n' || str[i] == '\r')
                break;
            h_state = h_key;
            key_start = i;
            now_read_line_begin = i;
            break;
        }
        case h_key:
        {
            if (str[i] == ':')
            {
                key_end = i;
                if (key_end - key_start <= 0)
                    return PARSE_HEADER_ERROR;
                h_state = h_colon;
            }
            else if (str[i] == '\n' || str[i] == '\r')
            {
                cout << "h_key" << endl;
                return PARSE_HEADER_ERROR;
            }
            break;
        }
        case h_colon:
        {
            if (str[i] == ' ')
            {
                h_state = h_spaces_after_colon;
            }
            else
                return PARSE_HEADER_ERROR;
            break;
        }
        case h_spaces_after_colon:
        {
            h_state = h_value;
            value_start = i;
            break;
        }
        case h_value:
        {
            if (str[i] == '\r')
            {
                h_state = h_CR;
                value_end = i;
                if (value_end - value_start <= 0)
                {
                    cout << "h_value" << endl;
                    return PARSE_HEADER_ERROR;
                }
            }
            else if (i - value_start > 255000)
            {
                cout << "i-value_start>255" << endl;
                return PARSE_HEADER_ERROR;
            }
            break;
        }
        case h_CR:
        {
            if (str[i] == '\n')
            {
                h_state = h_LF;
                string key(str.begin() + key_start, str.begin() + key_end);
                string value(str.begin() + value_start, str.begin() + value_end);
                headers[key] = value;
                now_read_line_begin = i;
            }
            else
            {
                cout << "h_CR" << endl;
                return PARSE_HEADER_ERROR;
            }
            break;
        }
        case h_LF:
        {
            if (str[i] == '\r')
            {
                h_state = h_end_CR;
            }
            else
            {
                key_start = i;
                h_state = h_key;
            }
            break;
        }
        case h_end_CR:
        {
            if (str[i] == '\n')
            {
                h_state = h_end_LF;
            }
            else
            {
                cout << "h_end_CR" << endl;
                return PARSE_HEADER_ERROR;
            }
            break;
        }
        case h_end_LF:
        {
            notFinish = false;
            key_start = i;
            now_read_line_begin = i;
            break;
        }
        }
    }
    if (h_state == h_end_LF)
    {
        str = str.substr(now_read_line_begin);
        cout << "parse Headers success" << endl;
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    cout << "PARSE_HEADER_AGAIN" << endl;
    return PARSE_HEADER_AGAIN;
}

int RequestData::parse_request()
{
    cout << "begin annalysis Request " << endl;
    if (method == METHOD_POST)
    {
        //get content
        char header[MAX_BUFF];
        sprintf(header, "HTTP/1.1 %d %s\r\n", 200, "OK");
        if (headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive")
        {
            keep_alive = true;
            sprintf(header, "%sConnection: keep-alive\r\n", header);
            sprintf(header, "%sKeep-Alive: timeout=%d\r\n", header, EPOLL_WAIT_TIME);
        }
        cout << "content=" << content << endl;
        // test char*
        char send_content[] = "I have receiced this.";

        sprintf(header, "%sContent-length: %zu\r\n", header, strlen(send_content));
        sprintf(header, "%s\r\n", header);
        size_t send_len = (size_t)writen(fd, header, strlen(header));
        if (send_len != strlen(header))
        {
            perror("Send header failed");
            return PARSE_REQUEST_ERROR;
        }

        send_len = (size_t)writen(fd, send_content, strlen(send_content));
        if (send_len != strlen(send_content))
        {
            perror("Send content failed");
            return PARSE_REQUEST_ERROR;
        }
        cout << "content size ==" << content.size() << endl;
        cout << "-----打印data-----" << endl;
        vector<char> data(content.begin(), content.end());
        for (vector<char>::iterator begin = data.begin(); begin != data.end(); ++begin)
        {
            cout << *begin << " ";
        }
        cout << "-----打印data完毕----" << endl;
        return PARSE_REQUEST_SUCCESS;
    }
    else if (method == METHOD_GET)
    {
        cout << "header" << endl;
        char header[MAX_BUFF];
        sprintf(header, "HTTP/1.1 %d %s\r\n", 200, "OK");
        if (headers.find("Connection") != headers.end() && headers["Connection"] == "keep-alive")
        {
            keep_alive = true;
            sprintf(header, "%sConnection: keep-alive\r\n", header);
            sprintf(header, "%sKeep-Alive: timeout=%d\r\n", header, EPOLL_WAIT_TIME);
        }
        int dot_pos = file_name.find('.');
        const char *filetype;
        if (dot_pos < 0)
            filetype = MimeType::getMime("default").c_str();
        else
            filetype = MimeType::getMime(file_name.substr(dot_pos)).c_str();
        struct stat sbuf;
        std::cout << "filetype:   " << filetype << std::endl;
        std::cout << "method get filename:" << file_name << std::endl;
        // std::cout << "sdsdas:" << stat(file_name.c_str(), &sbuf) << std::endl;
        if (stat(file_name.c_str(), &sbuf) < 0)
        {
            handle_error(fd, 404, "Not Found!");
            // file_name = "404.html";
            // std::cout << "error found :" << file_name << endl;
            return PARSE_REQUEST_ERROR;
        }

        sprintf(header, "%sContent-type: %s\r\n", header, filetype);
        // 通过Content-length返回文件大小
        sprintf(header, "%sContent-length: %ld\r\n", header, sbuf.st_size);

        sprintf(header, "%s\r\n", header);
        size_t send_len = (size_t)writen(fd, header, strlen(header));
        if (send_len != strlen(header))
        {
            perror("Send header failed");
            return PARSE_REQUEST_ERROR;
        }
        int src_fd = open(file_name.c_str(), O_RDONLY, 0);
        char *src_addr = static_cast<char *>(mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0));
        close(src_fd);

        // 发送文件并校验完整性
        send_len = writen(fd, src_addr, sbuf.st_size);
        if (send_len != sbuf.st_size)
        {
            perror("Send file failed");
            return PARSE_REQUEST_ERROR;
        }
        munmap(src_addr, sbuf.st_size);
        return PARSE_REQUEST_SUCCESS;
    }
    else
        return PARSE_REQUEST_ERROR;
}

void RequestData::handle_error(int fd, int err_num, string short_msg)
{
    cout << "begin handle_error ..." << endl;
    short_msg = " " + short_msg;
    char send_buff[MAX_BUFF];
    string body_buff, header_buff;
    body_buff += "<html><title>Miniweb Error</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    // body_buff += "<h1>404 Not Found!</h1>";
    std::string error_buf = to_string(err_num) + short_msg;
    body_buff += "<h1 style=\" text - align : center \">" + error_buf + "</h1>";
    // body_buff += "<hr><em> mianhk's Web Server</em>\n</body></html>";
    body_buff += "<p style=\" text - align : center \">Error from Miniweb by <a href=\" https://github.com/mianhk/miniweb\">mianhk</a>.</p>";
    header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-type: text/html\r\n";
    header_buff += "Connection: close\r\n";
    header_buff += "Content-length: " + to_string(body_buff.size()) + "\r\n";
    header_buff += "\r\n";
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

MutexLockGuard::MutexLockGuard()
{
    pthread_mutex_lock(&lock);
}

MutexLockGuard::~MutexLockGuard()
{
    pthread_mutex_unlock(&lock);
}
