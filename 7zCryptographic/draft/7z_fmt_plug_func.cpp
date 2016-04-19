//
// Created by shuai on 16/3/30.
//
#include <iostream>
#include <string.h>
//#include <algorithm>
//#include <stdio.h>
//#include <memory.h>
#define ARCH(x) ((unsigned int)(unsigned char)(x))
using namespace std;
char atoi16[0x100];
void init(){
    memset(atoi16,0x7F,sizeof(atoi16));
}
static int ishex(char* q){
    while (atoi16[ARCH(*q)] != 0x7F)
        q++;
    return !*q;// return the head of pointer
}
char * str1 = "$7z$12308";
const char * str2 = "$7z$9823";
int main(){
    init();
    char t[] = {'0','x','5','7'};
    char *q = t;
//    char *q = "0x7";
    char str3[] = "$7z$12308";
    char *p;
    str1 += 4;
    p = strtok(str3,"$");
    cout<<p<<strlen(p)<<endl;
    cout<<str1<<endl;
//    cout<<strncmp(str1,str2,5)<<endl;
//    cout<<ishex(q)<<endl;
    return 0;
}