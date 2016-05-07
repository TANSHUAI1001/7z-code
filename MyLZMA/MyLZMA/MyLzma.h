#ifndef MYLZMA_H
#define MYLZMA_H
#include"LzmaLib.h"
#pragma comment(lib,"LzmaLib.lib")
bool LzmaCompress(const char*scrfilename,const char*desfilename);
bool LzmaUncompress(const char*scrfilename,const char*desfilename);
bool MyLzmaUncompress(const char*scrfilename,const char*desfilename);
#endif