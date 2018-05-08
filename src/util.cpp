#include "util.h"
//map<string, int> config_map;

//int error_output(int error_num,string error_info){
//   printf("error: %s\n",error_info);
//   exit(error_num);
//}
using namespace std;

int read_config(const char* path){
   fstream config_file(path,fstream::in);
   string line,word;
   if(!config_file){
        printf("can't open %s\n",path);
        config_file.close();
        return(CONFIG_ERROR);
   }
   while(getline(config_file,line)){
       if("end"==line){
           cout<<"read end"<<endl;
           return 0;
       }
       string temp[10];
        temp=line.split("=");
       if(temp.size()>2){
           cout<<"error in config file"<<endl;
           return CONFIG_ERROR; 
       } 
        if("port"==temp[0]){
            if((int(temp[1])>10000)&&(int(temp[1]))<1024){
                cout<<"port error"<<endl;
                return CONFIG_ERROR;
            }
            port=temp[1];
        }
        if("webdir"==temp[0]){
            webdir=temp[1]
        }
        cout<<line<<endl; 
   }
   /*while(getline(config_file,line)){
        stringstream stream(line);
        stream>>word;
        string s;
        if("end"==line){
            printf("文件读取结束\n");
            exit;
        }

        printf("%s\n",word);
   }*/

    



}


