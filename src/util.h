#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
//#include <sys/socket.h>
//#include <sys/types.h>
#include <string>
#include <map>
#include <fstream>
//int error_output(int,string);
#define CONFIG_ERROR -1
static int port=5000;
static string webdir="../www"
int read_config(const char* path);
