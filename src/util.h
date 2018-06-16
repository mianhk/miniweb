<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
//#include <sys/socket.h>
//#include <sys/types.h>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
//int error_output(int,string);
#define CONFIG_ERROR -1
#define INIT_PORT 6000
#define INIT_WEB_DIR ../www
//static int port=5000;
//static std::string webdir="../www";

void handle_for_sigpipe();
int socket_bind_listen(int port); 

int set_socket_block(int fd);
=======
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
//#include <sys/socket.h>
//#include <sys/types.h>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
//int error_output(int,string);
#define CONFIG_ERROR -1
#define INIT_PORT 6000
#define INIT_WEB_DIR ../www
//static int port=5000;
//static std::string webdir="../www";

void handle_for_sigpipe();
int socket_bind_listen(int port); 

int set_socket_block(int fd);
>>>>>>> 1245ba3f16ae99ced5c12db10ef08baba47f04dc
