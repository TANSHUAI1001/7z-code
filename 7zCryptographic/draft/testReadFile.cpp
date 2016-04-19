//
// Created by shuai on 16/3/31.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "readFile.h"
using namespace std;
void stringToUnicode(char *src,char *dst)
{
    int len = strlen((char*)src);
    int i = 0;
    for (i = 0;i < len; i++)
    {
        dst[2*i] = src[i];
        dst[2*i + 1] = '\0';
    }
}
bool TextEncode(const char *fPath)
{
    char srcBuff[1024];
    unsigned char testBuff[1024];
//    unsigned int testBuff[1024];
    char header[2];
    unsigned char uniTxt[] = {0xFF, 0xFE};			// Unicode file header
    unsigned char endianTxt[] = {0xFE, 0xFF};		// Unicode big endian file header
    unsigned char utf8Txt[] = {0xEF, 0xBB, 0xBF};	// UTF_8 file header
    int len = 0;
    int ascii = 0;

    FILE *pFile;
    pFile = fopen(fPath, "rb");
    if (NULL == pFile)
    {
        return false;
    }

    //  ASCII range(0~127)
    fread(testBuff, 1, 1024, pFile);
//    printf("%c\n",srcBuff[0]);
//    printf("%c\n",srcBuff[1]);
//    printf("%c\n",srcBuff[2]);
    int testNum = 0x1D;
    testBuff[testNum] = '\0';
    for(int i = 0; i < testNum; i++){
        cout<<hex<<i<<":"<<int(testBuff[i])<<endl;
//        cout<<int(testBuff[i])<<endl;
    }
    cout<<(int)testBuff[8]<<" "<<(int)testBuff[0xC]<<" "<<(int)testBuff[0x14]<<" "<<(int)testBuff[0x1C]<<endl;
    //mbstowcs function test
//    size_t tSize = strlen(testBuff);
//    wchar_t *dBuff = NULL;
//    int dSize=mbstowcs(dBuff, testBuff, 0)+1;
//    dBuff = new wchar_t [dSize];
//    wmemset(dBuff,0,dSize);
//    int nRet=mbstowcs(dBuff, testBuff, tSize);
//    if(nRet<=0)
//    {
//        printf("转换失败\n");
//    }
//    else
//    {
//        printf("转换成功%d字符\n", nRet);
//        wprintf(L"%ls\n", dBuff);
//    }
//// wcstombs function test
//    wchar_t sBuf[20]={0};
//    wcscpy(sBuf, L"Hello");
//    size_t sSize=wcslen(sBuf);
//
//
//    char * dBuf=NULL;
//    int dbSize=wcstombs(dBuf, sBuf, 0)+1;
//    printf("需要%d Char\n", dbSize);
//
//    dBuf=new char[dbSize];
//    memset(dBuf, 0, dbSize);
//
//
//    int n2Ret=wcstombs(dBuf, sBuf, dbSize);
//
//    if(n2Ret<=0)
//    {
//        printf("转换失败\n");
//    }
//    else
//    {
//        printf("转换成功%d字符\n", nRet);
//        printf("%s\n", dBuf);
//    }
//    cout<<__need_wchar_t(testBuff)<<endl;
//    cout<<strlen(testBuff)<<endl;
//    char * dest = (char *)malloc((strlen(testBuff)*2+1)*sizeof(char));
//    cout<<strlen(dest)<<endl;
//    unsigned char * tt = &(unsigned char)testBuff;
//    unsigned long * dest = new unsigned long [10];
//    enc_utf8_to_unicode_one(testBuff,dest);
//    stringToUnicode(testBuff,dest);
//    cout<<hex<<dest<<endl;
//    cout<<srcBuff[0]<<srcBuff[1]<<srcBuff[2]<<srcBuff[3]<<srcBuff[4]<<endl;
//    free(dest);
    while (1)
    {
        len = fread(srcBuff, 1, 1024, pFile);
        if (0 == len)
        {
            break;
        }
//        printf("%c\n",srcBuff[0]);
//        printf("%c\n",srcBuff[1]);
//        printf("%c\n",srcBuff[2]);
        for (int i=0; i<len; i++)
        {
            header[0] = srcBuff[0];
            header[1] = srcBuff[1];
            header[2] = srcBuff[2];

            if (srcBuff[i]<0 || srcBuff[i]>127)
            {
                ascii++;
            }

        }
    }

    if (0 == ascii)		// ASCII file
    {
        printf("ASCII text\n");
    }
    else if ((2 == ascii) && (0 == memcmp(header, uniTxt, sizeof(uniTxt))))		// Unicode file
    {
        printf("Unicode text\n");
    }
    else if ((2 == ascii) && (0 == memcmp(header, endianTxt, sizeof(endianTxt))))	//  Unicode big endian file
    {
        printf("Unicode big endian text\n");
    }
    else if ((3 == ascii) && (0 == memcmp(header, utf8Txt, sizeof(utf8Txt))))		// UTF-8 file
    {
        printf("UTF-8 text\n");
    }
    else
    {
        printf("	Unknow\n");
    }
    fclose(pFile);
    return true;
}

int main()
{
    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA_FileName_Password.7z";
    cout<<TextEncode(path)<<endl;
    return 0;
}