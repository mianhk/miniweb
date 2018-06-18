# Miniweb
Miniweb is a lightwight HTTP WebServer based on C++11.
## Features
- Use Epoll with ET mode on Linux  
- Use fixed number of thread poll  
- Use task queue triggered by condition variable  
- Use smart pointer  
## Tested Envoirment
- OS Platform : Ubuntu 16.04.4 LTS  
- Complier : gcc version 5.4.0
## Demo Page
Please go to: www.yuguocong.cn/miniweb
<img src="http://blog-1252063226.cosbj.myqcloud.com/network/20180618201015.png"/>
## Getting started
```
# clone reposity to your local path.
git clone git@github.com:mianhk/miniweb.git

# modify config file to change port and so on.
port=5000
path="./www"
thread_num=4

# change to dir
cd src

# compile source code
make

# run miniweb
./miniweb

# or if you want run miniweb background
./miniserver 1>>log &    # the output of the webserver will be in log file
```
## Source Code
## What's Next
- Only solves GET request,need to do more  
 
