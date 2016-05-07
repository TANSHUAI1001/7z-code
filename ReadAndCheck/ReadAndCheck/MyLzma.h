#ifndef MYLZMA_H
#define MYLZMA_H
#include"LzmaLib.h"
#pragma comment(lib,"LzmaLib.lib")
bool LzmaCompress(const char*scrfilename,const char*desfilename);
bool LzmaUncompress(const char*scrfilename,const char*desfilename);
extern bool MyLzmaUncompress(const char*scrfilename,unsigned char *&outBuff,size_t propsSize,
					  unsigned char props,int inSize,int outSize);
#endif