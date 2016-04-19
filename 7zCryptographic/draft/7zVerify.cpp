//
// Created by shuai on 16/3/20.
//

#include <string.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <assert.h>
// #include <malloc.h>
#include <mm_malloc.h>//In the OSX system envirenment, modified by tanshuai

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long unit64;

#define GetUi32(p) (*(const uint32 *)p)
#define SetUi32(p,d) *(uint32 *)(p) = (d)



int main(){

    uint8 a = 'a';
    uint8 *ta = &a;
    printf("%c",*ta);
    return 0;
}