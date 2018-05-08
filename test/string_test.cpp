#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc,char** argv){

    if(argc!=2){
    printf("")}
    //string temp[2];
    vector<string> temp(3);
    //cout<<"sdhjkahs"<<endl;
    temp[0]="ksi shie hqk";
    temp[1]="sdha llluk qgweb";
    //cout<<temp.size()<<endl;
    for(int i=0;i<temp.size();i++){
        cout<<temp[i]<<endl;
       // vector<string> tt;
       string tt[10];
       tt=temp[i].split(" ");
       for(int j=0;j<10;j++){
           cout<<tt[j]<<endl;
       }
        
    }
    return 0;
}
