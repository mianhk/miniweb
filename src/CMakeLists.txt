cmake_minimum_required(VERSION 3.8)
project(src)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
#        util.cpp
#        util.h
        epoll.cpp epoll.h http_request.cpp http_request.h list.h threadpoll.cpp threadpoll.h)
add_executable(src ${SOURCE_FILES})