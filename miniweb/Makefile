# MAINSOURCE代表含有main入口函数的cpp文件，因为含有测试代码，
# 所以要为多个目标编译，这里把Makefile写的通用了一点，
# 以后加东西Makefile不用做多少改动
MAINSOURCE := main.cpp test/client.cpp
# MAINOBJS := $(patsubst %.cpp,%.o,$(MAINSOURCE))
SOURCE  := $(wildcard *.cpp test/*.cpp)
override SOURCE := $(filter-out $(MAINSOURCE),$(SOURCE))
OBJS    := $(patsubst %.cpp,%.o,$(SOURCE))

TARGET  := miniserver
CC      := g++
LIBS    := -lpthread 
INCLUDE:= -I./usr/local/lib
CFLAGS  := -std=c++11 -g -Wall -O3 -D_PTHREADS
CXXFLAGS:= $(CFLAGS)

# Test object
SUBTARGET := client

.PHONY : objs clean veryclean rebuild all tests debug
all : $(TARGET) $(SUBTARGET) 
objs : $(OBJS)
rebuild: veryclean all

tests : $(SUBTARGET) 
clean :
	find . -name '*.o' | xargs rm -f
veryclean :
	find . -name '*.o' | xargs rm -f
	find . -name $(TARGET) | xargs rm -f
	find . -name $(SUBTARGET) | xargs rm -f
debug:
	@echo $(SOURCE)

$(TARGET) : $(OBJS) main.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
# $@代表目标，这里是$(TARGET)

$(SUBTARGET) : $(OBJS) test/client.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)
