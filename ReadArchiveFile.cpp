//
// Created by shuai on 16/4/8.
//
#include <iostream>

using namespace std;
//#define MAGIC_7Z '0x370x7a\xbc\xaf\x27\x1c'
//unsigned int MAGIC_7Z = '0x370x7a\xbc\xaf\x27\x1c';
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define PACK_INFO 0x06
#define UNPACK_INFO 0x07
#define SUBSTEAMS_INFO 0x08
#define PROPERTY_SIZE 0x09
#define PROPERTY_END 0x00
#define PROPERTY_CRC 0x0a
#define MASK 0x80

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef unsigned long DWORD;
typedef unsigned long long FWORD;
union IntChar32{
    unsigned char char_v[4];
    unsigned int int_v;
};
union  LongChar64{
    unsigned char char_v[8];
    unsigned long long_v;
};
WORD ConvertEndian(WORD dwBigEndian)
{
    BYTE* p = (BYTE*)&dwBigEndian;
//    cout<<hex<<(int)p[0]<<endl;
    return (WORD)(p[0] << 24) + (WORD)(p[1] << 16) +
           (WORD)(p[2] << 8) + (WORD)p[3];
}
DWORD DConvertEndian(DWORD dwBigEndian)
{
    WORD * p = (WORD *)&dwBigEndian;

    return (DWORD)(ConvertEndian(p[0]) << 32) + (DWORD)(ConvertEndian(p[1]));
//    return (DWORD)(ConvertEndian(p[0])) + (DWORD)(ConvertEndian(p[1]) << 32);

//    cout<<hex<<int(p[1] << 48)<<endl;
//    return (FWORD)(p[0] << 56) + (FWORD)(p[1] << 48) + (FWORD)(p[2] << 40) + (FWORD)(p[3] << 32) +
//            (FWORD)(p[4] << 24) + (FWORD)(p[5] << 16) +(FWORD)(p[6] << 8) + (FWORD)p[7];
//    return (DWORD)(p[0] << 32) | (DWORD)p[1];
}
string get_b2hex(const unsigned char * source,int len)
{
    string strHexPack;
    for (int i = 0; i < len; ++i)
    {
        unsigned char c = source[i];
        unsigned int nIntVal = c;
        char hex_buf[10] = {0};
        sprintf(hex_buf, "%02x", nIntVal);
        strHexPack += hex_buf;
    }
    return strHexPack;
//printf("*********strHexPack:%s/n", strHexPack.c_str());
}
long readHandle64Bit(long curPos,unsigned char * pBuf,FILE *pFile){
    cout<<"handle 64 bit!"<<endl;
    int i = 0,cMASK;
    cMASK = MASK;
    int cut = 0x80;
    int realCut;
    while(cMASK){
//        cout<<curPos<<"  "<<cMASK<<endl;
        if((curPos & cMASK) != 0) {
            i++;
            cut >>= 1;
            realCut = cut;
        }
        else{
            cMASK = 0;
        }
        cMASK >>= 1;
    }
    while(cut >>= 1){
        realCut |= cut;
    }
    long highPart = curPos & realCut;
    long lowPart = 0;
    for(int j = i; j > 0; j--){
        fread(pBuf,1,1,pFile);
        lowPart += int(pBuf[0]) << ((j-1) * 8);
    }
    long packPos = ((highPart << (i*8)) + lowPart);
//    cout<<"packPos "<<packPos<<endl;
    return packPos;
}
int ReadByPath(char *fp)
{
    int i = 0;
    FILE *pFile;
    pFile = fopen(fp, "rb");
    if (NULL == pFile)
    {
        return 0;
    }
    unsigned char buffer[1024];
//    unsigned long lBuffer[1024];
//    unsigned int iBuffer[1024];
    unsigned char *pBuff;
    pBuff = buffer;


    fread(buffer,1,6,pFile); // read 7z HeadText == MAGIC_7Z
    for(i = 0; i < 6; i++){
        cout<<hex<<i<<":"<<int(buffer[i])<<endl;
    }
    fread(buffer,1,2,pFile); //version
    for(i = 0; i<2;i++){
        cout<<hex<<int(buffer[i])<<endl;
    }
    IntChar32 buf;
    fread(buf.char_v,1,4,pFile); //startHeaderCrc
    cout<<"###"<<buf.int_v<<endl;
    unsigned int C_SHC = ConvertEndian(buf.int_v);
    cout<<C_SHC<<endl;
    LongChar64 lc_buff;
    fread(lc_buff.char_v,1,8,pFile); //
    cout<<"nextHeaderOffset"<<lc_buff.long_v<<endl;
    long nextHeaderOffset = lc_buff.long_v;
    unsigned long NHOffset =  DConvertEndian(lc_buff.long_v);  // did not check !!!
//    cout<<hex<<lc_buff.char_v<<endl;
    cout<<hex<<NHOffset<<endl;
    //CRC check part have not written
    LongChar64 getNHS;
    fread(getNHS.char_v,1,8,pFile);
    cout<<"nextHeaderSize"<<getNHS.long_v<<endl;
    long nextHeaderSize = getNHS.long_v;
    IntChar32 NHCrc;
    fread(NHCrc.char_v,1,4,pFile);
    cout<<"NextHeaderCrc"<<NHCrc.int_v<<endl;
    int nextHeaderCrc = NHCrc.int_v;
    long afterHeader = ftell(pFile);
    cout<<"afterHeader position is "<<afterHeader<<endl;

    /* fseek
    * 第三个参数 ,SEEK_SET： 文件开头,SEEK_CUR： 当前位置,SEEK_END： 文件结尾
    * 其中SEEK_SET,SEEK_CUR和SEEK_END依次为0，1和2.
    */
    int successSeek = fseek(pFile,nextHeaderOffset,SEEK_CUR);
    long tailHeaderBegin = ftell(pFile);
    cout<<"tailHeaderBegin "<<tailHeaderBegin<<endl;
    int seekEnd = fseek(pFile,0L,SEEK_END);
    long endPosition = ftell(pFile);
    cout<<"seekEnd "<<seekEnd<<" endPosition "<<endPosition<<endl;
    cout<<"is success seek ? "<<successSeek<<endl; // 成功返回0,否则一个非零值
    long tailOffset = (endPosition - tailHeaderBegin);
    cout<<"tailOffset "<<dec<<tailOffset<<endl;
    fseek(pFile,tailHeaderBegin,SEEK_SET);
    unsigned char tailBuffer[tailOffset];
    fread(tailBuffer,1,tailOffset,pFile);
    const unsigned char *pTailBuf;
    pTailBuf = tailBuffer;
    cout<<"tail hex "<<get_b2hex(pTailBuf,tailOffset)<<endl;

    fseek(pFile,tailHeaderBegin,SEEK_SET);
    long k = 0;
    IntChar32 streamBuf;
    while(k < tailOffset){
        fread(buffer,1,1,pFile);
        int curPos = int(buffer[0]);
        cout<<hex<<"before pack info "<<curPos<<endl;
        if(curPos == PACK_INFO){
            long packPos,numStream;
//            IntChar32 b;
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
//            int MASK = 0x80;
            // pack position
            if((curPos & MASK) == 0){
                packPos = streamBuf.int_v;
                cout<<"packPos"<<packPos<<endl;
            }
            if((curPos & MASK) != 0){
                cout<<"maybe need to handle this byte !"<<endl;
                packPos = readHandle64Bit(curPos,pBuff,pFile);
                /*
                int i = 0,cMASK;
                cMASK = MASK;
                int cut = MASK;
                int realCut;
                while(cMASK){
                    cout<<curPos<<"  "<<cMASK<<endl;
                    if((curPos & cMASK) != 0) {
                        i++;
                        cut >>= 1;
                        realCut = cut;
//                        cout<<"RC"<<realCut<<endl;
                    }
                    else{
                        cMASK = 0;
                    }
                    cMASK >>= 1;
                }
                while(cut >>= 1){
                    realCut |= cut;
                }
//                cout<<"cut"<<realCut<<endl;
                int highPart = curPos & realCut;
//                cout<<"HP "<<highPart<<endl;
//                cout<<"i"<<i<<endl;
                int lowPart = 0;
                for(int j = i; j > 0; j--){
                    fread(buffer,1,1,pFile);
//                    cout<<"bf0 "<<int(buffer[0])<<endl;
                    lowPart += int(buffer[0]) << ((j-1) * 8);
                }
//                cout<<"LP "<<lowPart<<endl;
                packPos = ((highPart << (i*8)) + lowPart);
                cout<<"packPos "<<packPos<<endl;
*/
            }

            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            if((curPos & MASK) == 0){
                numStream = curPos;
                cout<<"numStream "<<numStream<<endl;
            }
            if((curPos & MASK) != 0){
                cout<<curPos<<endl;
                cout<<"maybe need to handle this byte !"<<endl;
                numStream = readHandle64Bit(curPos,pBuff,pFile);
            }
            long packSize[numStream];
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            cout<<"ID"<<curPos<<endl;
            if(curPos == PROPERTY_SIZE){
                for(int i = 0; i < numStream;i++){
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                    if((curPos & MASK) == 0){
                        packSize[i] = curPos;
                    }
                    if((curPos & MASK) != 0){
                        packSize[i] = readHandle64Bit(curPos,pBuff,pFile);
                    }
//                    cout<<packSize[i]<<endl;
                }

                cout<<"wait to write"<<endl;
                fread(buffer,1,1,pFile);
                curPos = int(buffer[0]);
                if(curPos == PROPERTY_CRC){
                    cout<<"getCRC"<<endl;
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                }
            }
            if(curPos != PROPERTY_END){
                cout<<"end id expected but "<<curPos<<" found !\n"<<endl;
            }
        }
        if(streamBuf.int_v == UNPACK_INFO){

        }
        if(streamBuf.int_v == SUBSTEAMS_INFO){

        }

        k++;
    }

    int close = fclose(pFile);
    cout<<"file is close ? "<<close<<endl;// 成功返回0,否则 EOF(-1)
    return 1;
}


int main(){
    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA_FileName_Password.7z";
    cout<<ReadByPath(path)<<endl;
    return 0;
}