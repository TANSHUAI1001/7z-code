//
// Created by shuai on 16/3/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "test.h"
using namespace std;

unsigned char crcc[4] = {'a', 'b', 'c', 'd'};
unsigned char* crc = crcc;

void init(struct Attr* a){
    a->power = 1;
    a->speed = 1;
    a->score = 1.0;
    return ;
}
int MyCatoi(char s[]){
    int i=0,n=0,sign=1;
    for(;isspace(s[i]);i++);//跳过空白符;
    if(s[i]=='+'||s[i]=='-')//跳过符号
    {
        sign=(s[i]=='-')?-1:1;
        cout<<"sign:"<<sign<<endl;
        i++;
    }
    for(;isdigit(s[i]);i++)
        n=10*n+(s[i]-'0');//将数字字符转换成整形数字
    return sign *n;
}

static int ishex(char *q)
{
//    while (atoi16[ARCH_INDEX(*q)] != 0x7F)
//        q++;
    cout<<*q<<endl;
    return !*q;
}
//static int isdecu(char *q)
//{
//    char buf[24];
////    unsigned int x = atou(q);
////    sprintf(buf, "%u", x);
//    return !strcmp(q,buf);
//}
//int main(){
//    printf("%s\n",crc);
//    cout<<crc<<endl;
//    int a[20];
//    int *ptr = a;
//
//    Attr test{1,2,3.0};
//    Attr bb;
//    init(&bb);
//    cout<<test.power<<test.score<<test.speed<<endl;
//    cout<<bb.power<<bb.score<<bb.speed<<endl;
//    Attr *pTest = &test;
//    cout<<pTest->speed<<pTest->score<<pTest->power<<endl;
//    char s2i[] = "0123";
//    char *p = s2i;
//    cout<<"ishex"<<ishex(p)<<endl;
//    int out = atoi(s2i);
//    int out1 = MyCatoi(s2i);
//    cout<<"out="<<out<<",out1="<<out1<<","<<(out==out1)<<endl;
//    return 0;
//}
//#include <iostream>

//using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
uint8_t gmult(uint8_t a, uint8_t b) {

    uint8_t p = 0, i = 0, hbs = 0;

    for (i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }

        hbs = a & 0x80;
        a <<= 1;
        if (hbs) a ^= 0x1b; // 0000 0001 0001 1011
        b >>= 1;
    }

    return (uint8_t)p;
}

uint8_t gadd(uint8_t a, uint8_t b) {
    return a^b;
}

int main()
{

//    char a = '0x0a'; // 'a'
//    char b = '\x0a'; // 十六进制对应所表示的字符,有些为不可见字符'\n'换行
//    char b1 = '\x0d';// '/r'回车
//    char b2 = '\b1010';
//    char b3 = '\12';
//    char b4 = '012';
//    char c = '\x03';
//    char d = 0x03;
//    int e = '0x03';
//    char f = char(c);
//    int g = '0x03';// 30(0) 78(x) 30(0) 33(3) ==> 813183027
//    int h = '3';// 33 ==> 51
//    int i = '\x03'; //5c(\) 78(x) 30(0) 33(3) ==> ??? == 3 '\x03'代表ETX,其ascii值为3
//    int j = a;
//    cout<<(int)gmult(3,9)<<endl;
    cout<<(int)gadd(3,5)<<endl;
//    cout<< sizeof(long)<< sizeof(int)<<endl;
//    cout << "Hello World" << endl;
//    unsigned char a[5] = {'\x0a','\x0b','\x0c','\x0d','\x0e'};
//    int size = 6;
//    unsigned char *b = new unsigned char[6];
//    for(int i = 0; i < 6;i++){
//
//        b[i] = a[i]?a[i]:'\x00';
//        cout<<(int)b[i]<<endl;
//    }
//    delete b;
    return 0;
}