CFLAGS=-std=c++11
XX=g++
miniweb:main.o util.o
	$(XX) *.o -o miniweb 

main.o:main.cpp
	$(XX) -c main.cpp

util.o:util.cpp util.h
	$(XX) -c util.cpp	


