#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <util.h>

map<string, int> config_map;

int error_output(int error_num,string error_info){
   printf("error: %s\n",error_info); 
   exit(error_num);
}

void read_config(const char* path){
    
}


