//
// Created by shuai on 16/4/8.
//
#include <iostream>
#include <vector>
#include "ReadProperties.h"
#include <zlib.h>
#include "Crc32.h"
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
#define PROPERTY_FOLDER 0x0b
#define CODER_UPACK_SIZE 0x0c
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
bool readBoolean(long curPos, unsigned char *pBuff,FILE *pFile){
    //
}
long readHandle64Bit(long curPos,unsigned char * pBuf,FILE *pFile){
    cout<<"handle 64 bit!"<<endl;
    if((curPos & MASK) == 0){
        return curPos;
    }
    int i = 0,cMASK;
    long highPart = 0x00;

//    cMASK = MASK;
//    int cut = 0x80;
//    int realCut;
//    while(cMASK){
//        cout<<curPos<<"  "<<cMASK<<endl;
//        if((curPos & cMASK) != 0) {
//            i++;
//            cut >>= 1;
//            realCut = cut;
//        }
//        else{
//            cMASK = 0;
//        }
//        cMASK >>= 1;
//    }
//    while(cut >>= 1){
//        realCut |= cut;
//    }
//    highPart = curPos & realCut;

//    highPart = curPos & (cut - 1);

    for(int num = 1; num < 8; num++){
        unsigned cMASK = (unsigned)MASK >> num;
        if((curPos & cMASK) == 0){
            highPart = curPos & (cMASK - 1);
            i = num;
            break;
        }
    }

    long lowPart = 0;
    for(int j = 0; j < i; j++){
        fread(pBuf,1,1,pFile);
        lowPart += int(pBuf[0]) << (j * 8);
    }
    long value = ((highPart << (i*8)) + lowPart);
    return value;
}
int ivSize,saltSize,numCyclesPower;
unsigned char * pIv;
unsigned char * pSalt;

void setDecoderProp(unsigned char * pProp,int len,CheckData *&pCd){
    int pos = 0;
    unsigned char firstByte = pProp[pos];
    pos += 1;
    numCyclesPower = firstByte & 0x3f;
    numCyclesPower = numCyclesPower < 24?numCyclesPower:-1;
    saltSize = (firstByte >> 7) & 1;
    ivSize = (firstByte >> 6) & 1;

    unsigned char secondByte = pProp[pos];
    pos += 1;
    saltSize += (secondByte >> 4);
    ivSize += (secondByte & 0x0f);
    pos += saltSize;
    if(saltSize != 0) {
        pSalt = new unsigned char[saltSize];
        for (int j = 0; j < saltSize; j++) {
            pSalt[j] = pProp[pos + j];
        }
        cout << endl;
    }
    else{
        saltSize = 2;
        pSalt = new unsigned char[2];
        pSalt[0] = '\x11';
        pSalt[1] = '\x22';
    }
//    pos += saltSize; // not the correct positon, just for a test;
    cout<<"scp "<<numCyclesPower<<" ss "<<saltSize<<" ivs "<<ivSize<<endl;
    for (int j = 0; j < saltSize; j++) {
        cout << (int)pSalt[j] << " ";
    }
    cout << endl;
    pIv = new unsigned char[16];
    for(int i = 0; i<16;i++){
//        pIv[i] = pProp[pos+i]?pProp[pos+i]:'\x00';
//        cout<<((pos+i)>(len-1))<<endl;
        pIv[i] = (pos+i) > (len-1) ? '\x00':pProp[pos+i];
        cout<<(int)pIv[i]<<" ";
    }
    cout<<endl;
    pCd->saltSize = saltSize;
    pCd->ivSize = ivSize;
    pCd->iv = pIv;
    pCd->salt = (char *)pSalt;

}
int ReadByPath(char *fp,CheckData *&pCd)
{
    int i = 0;
    FILE *pFile;
    pFile = fopen(fp, "rb");
    if (NULL == pFile)
    {
        return 0;
    }
    unsigned char buffer[1024];
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
    unsigned long startHeaderCrc = buf.int_v;
    cout<<"startHeaderCrc:"<<startHeaderCrc<<endl;

    LongChar64 lc_buff;
    fread(lc_buff.char_v,1,8,pFile); //
    cout<<"nextHeaderOffset"<<lc_buff.long_v<<endl;
    long nextHeaderOffset = lc_buff.long_v;  // it is read by little endian

    //CRC check part
//    unsigned int value;
//    unsigned int *crc = &value;
    unsigned int crc;
    unsigned char outValue[4];
    unsigned char *out = outValue;
    CRC32_Init(&crc);
    CRC32_Update(&crc,lc_buff.char_v,8);

    LongChar64 getNHS;
    fread(getNHS.char_v,1,8,pFile);
    cout<<"nextHeaderSize"<<getNHS.long_v<<endl;
    long nextHeaderSize = getNHS.long_v;
    CRC32_Update(&crc,getNHS.char_v,8);

    IntChar32 NHCrc;
    fread(NHCrc.char_v,1,4,pFile);
    cout<<"NextHeaderCrc"<<NHCrc.int_v<<endl;
    int nextHeaderCrc = NHCrc.int_v;
    CRC32_Update(&crc,NHCrc.char_v,4);
    cout<<"after 3 CRC32 update"<<crc<<endl<<(crc & 0xffffffff)<<endl;
    CRC32_Final(out,crc);
    IntChar32 outInt;
//    outInt.char_v = out;
    cout<<"out :"<<endl;
    for(int i = 0; i < 4;i++){
        outInt.char_v[i] = out[i];
        cout<<(int)out[i]<<" ";
    }
    cout<<endl;
    cout<<outInt.int_v<<endl;
    cout<<"check equals ? "<<(outInt.int_v == startHeaderCrc)<<endl;
    long afterHeader = ftell(pFile);
    cout<<"afterHeader position is "<<afterHeader<<endl;

    /* fseek
    * 第三个参数 ,SEEK_SET： 文件开头,SEEK_CUR： 当前位置,SEEK_END： 文件结尾
    * 其中SEEK_SET,SEEK_CUR和SEEK_END依次为0，1和2.
    */
    int successSeek = fseek(pFile,nextHeaderOffset,SEEK_CUR);
    cout<<"is success seek ? "<<successSeek<<endl; // 成功返回0,否则一个非零值

    long tailHeaderBegin = ftell(pFile);
    cout<<"tailHeaderBegin "<<tailHeaderBegin<<endl;
    int seekEnd = fseek(pFile,0L,SEEK_END);
    long endPosition = ftell(pFile);
    cout<<"seekEnd "<<seekEnd<<" endPosition "<<endPosition<<endl;
    long tailOffset = (endPosition - tailHeaderBegin);
    cout<<"tailOffset dec = "<<dec<<tailOffset<<endl;
    fseek(pFile,tailHeaderBegin,SEEK_SET);
    unsigned char tailBuffer[tailOffset];
    fread(tailBuffer,1,tailOffset,pFile);
    const unsigned char *pTailBuf;
    pTailBuf = tailBuffer;
    cout<<"tail hex "<<get_b2hex(pTailBuf,tailOffset)<<endl;

    fseek(pFile,tailHeaderBegin,SEEK_SET);
    long k = 0;
    IntChar32 streamBuf;
    StreamsData streamsData;
    while(k < tailOffset){
        fread(buffer,1,1,pFile);
        int curPos = int(buffer[0]);
        if(curPos == 0x17)
            continue;
//        cout<<hex<<"before pack info "<<curPos<<endl;
        if(curPos == PACK_INFO){ //0x06
            long packPos,numStream;
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            // pack position
            packPos = readHandle64Bit(curPos,pBuff,pFile);
            cout<<"packPos"<<packPos<<endl;
            /*
            if((curPos & MASK) == 0){
                packPos = curPos;
//                cout<<"packPos"<<packPos<<endl;
            }
            if((curPos & MASK) != 0){
                cout<<"maybe need to handle this byte !"<<endl;
                packPos = readHandle64Bit(curPos,pBuff,pFile);

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

            }
             */

            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            numStream = readHandle64Bit(curPos,pBuff,pFile);
            cout<<numStream<<endl;
            streamsData.pPackSizes = new long[numStream];
            long *packSize = streamsData.pPackSizes;
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
//            cout<<"ID"<<curPos<<endl;
            if(curPos == PROPERTY_SIZE){ // 0x09
                for(int i = 0; i < numStream;i++){
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                    packSize[i] = readHandle64Bit(curPos,pBuff,pFile);
//                    cout<<packSize[i]<<endl;
                }

                cout<<"wait to write"<<endl;
                fread(buffer,1,1,pFile);
                curPos = int(buffer[0]);
                if(curPos == PROPERTY_CRC){ //0x0a
                    cout<<"getCRC"<<endl;
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                }
            }
            if(curPos != PROPERTY_END){ //0x00
                cout<<"end id expected but "<<curPos<<" found !\n"<<endl;
            }
            streamsData.packPos = packPos;
            streamsData.numStreams = numStream;
        }
        if(curPos == UNPACK_INFO){ // 0x07
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            if(curPos == PROPERTY_FOLDER){  //0x0b
                cout<<"it correctly is folder !"<<endl;
            }
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            long numFolders = readHandle64Bit(curPos,pBuff,pFile);
            cout<<"numFolders "<<numFolders<<endl;
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            streamsData.pFolders = new Folders[numFolders];
            Folders *folders = streamsData.pFolders;
            long dataStreamIdx = 0x00;
            if(curPos == 0x00){
                for(int i = 0; i < numFolders; i++){
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                    long numCoders = readHandle64Bit(curPos,pBuff,pFile);
                    bool digestDefined = false;
                    long totalIn = 0;
                    long totalOut = 0;
                    streamsData.pFolders[i].pCoders = new Coders[numCoders];
                    Coders *coders = streamsData.pFolders[i].pCoders;

                    cout<<"numCoders "<<numCoders<<endl;
                    folders[i].numCoders = numCoders;
                    cout<<"numCoders "<<streamsData.pFolders[i].numCoders<<endl;

                    for(int j = 0;j < numCoders;j++){
                        while(true){  // single need not this, but multiple needs !!!
                        fread(buffer,1,1,pFile);
                        curPos = int(buffer[0]);
                        int b = curPos;

                        int methodSize = b & 0xf;
                        bool isSimple = ((b & 0x10) == 0);
                        bool noAttributes = ((b & 0x20) == 0);
                        bool lastAlternative = ((b & 0x80) == 0);
                        cout<<"methodSize "<<methodSize<<endl;
                        fread(buffer,1,methodSize,pFile);
//                            curPos = int(buffer[0]);
                        long method = 0x00;
                        for(int s = 0; s < methodSize;s++){
                            method += buffer[s] << ((methodSize - s - 1) * 8);
                        }
                        cout<<"method "<<method<<endl;  // some probrems
                        coders[j].method = method;
                        long minInstreams = 1, minOutstreams = 1;
                        if(!isSimple){
                            fread(buffer,1,1,pFile);
                            curPos = int(buffer[0]);
                            minInstreams = readHandle64Bit(curPos,pBuff,pFile);
                            fread(buffer,1,1,pFile);
                            curPos = int(buffer[0]);
                            minOutstreams = readHandle64Bit(curPos,pBuff,pFile);
                        }
                        totalIn += minInstreams;
                        totalOut += minOutstreams;
                        long propertiesSize = 0x00;

                        if(!noAttributes){
                            fread(buffer,1,1,pFile);
                            curPos = int(buffer[0]);
                            propertiesSize = readHandle64Bit(curPos,pBuff,pFile);
                            fread(buffer,1,propertiesSize,pFile);
                            cout<<"properties:"<<get_b2hex(pBuff,propertiesSize)<<endl;
                            if(streamsData.pFolders[i].pCoders[i].propertiesSize != 0 ) continue;
                            streamsData.pFolders[i].pCoders[i].propertiesSize = propertiesSize;
                            streamsData.pFolders[i].pCoders[j].pProp = new unsigned char[propertiesSize];
                            for(int n = 0; n < propertiesSize; n++){
                                streamsData.pFolders[i].pCoders[j].pProp[n] = buffer[n];
                            }

                        }
//                            cout<<"properties "<<propertiesSize<<" "<<streamsData.pFolders[i].pCoders[j].pProp<<endl;
//                            for(int n = 0; n < propertiesSize; n++){
//                                cout<<(int)streamsData.pFolders[i].pCoders[j].pProp[n]<<" ";
//                            }
//                            coders[j].properties = properties;
                        if(lastAlternative) break;
                        }/* end while (true)*/
                    }
                    folders[i].digestDefined = digestDefined;
                    streamsData.totalOut = totalOut;
                    folders[i].pCoders = coders;
                    folders[i].numPackedStream = totalIn - totalOut + 1;

                    // i think these do not used !
                    /*
                    long numBindPairs = totalOut - 1;
                    cout<<"numBindPairs "<<numBindPairs<<endl;
                    long bindPairs[numBindPairs][2];
                    for(int num = 0; num < numBindPairs;num++){
                        fread(buffer,1,1,pFile);
                        curPos = int(buffer[0]);
                        bindPairs[num][0] = readHandle64Bit(curPos,pBuff,pFile);
                        fread(buffer,1,1,pFile);
                        curPos = int(buffer[0]);
                        bindPairs[num][1] = readHandle64Bit(curPos,pBuff,pFile);
                    }
                    long numPackedStream = totalIn - numBindPairs;
                    vector<long> packedIndexes;
                    if(numPackedStream == 1){
                        for(int num = 0;num < totalIn;num++){
                            if(1){
                                packedIndexes.push_back(i);
                            }
                        }
                    }
                    else if(numPackedStream > 1){
                        for(int num = 0; num < numPackedStream;num++){
                            fread(buffer,1,1,pFile);
                            curPos = int(buffer[0]);
                            packedIndexes.push_back(readHandle64Bit(curPos,pBuff,pFile));
                        }
                    }
                    cout<<"size"<<packedIndexes.size()<<endl;
                    for(int i = 0; i < packedIndexes.size();i++){
                        cout<<"packedIndices"<<packedIndexes[i]<<endl;
                    }
                     */
                }
            }
            else if(curPos == 0x01){
                fread(buffer,1,1,pFile);
                curPos = int(buffer[0]);
                dataStreamIdx = readHandle64Bit(curPos,pBuff,pFile);
                cout<<"dsi "<<dataStreamIdx<<endl;
                streamsData.dataStreamIdx = dataStreamIdx;
            }
            else{
                cout<<"none-expected byte!"<<endl;
            }
            streamsData.numFolders = numFolders;
            //coders_upack_size
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            if(curPos == CODER_UPACK_SIZE){// 0x0c
                cout<<"coder_unpack_size !"<<endl;
//                fread(buffer,1,1,pFile);
//                curPos = int(buffer[0]);
//                int numOfUnpacksizes = 1;
//                int numOfUnpacksizes = streamsData.totalOut - 1;
                // it may not be totalOut in multi-file /multiple unpacksizes
                int numOfUnpacksizes = streamsData.pFolders->numCoders;
                // it may not be totalOut in multi-file /multiple unpacksizes
                streamsData.pUnpackSizes = new long[numOfUnpacksizes];
                long *unpackSizes = streamsData.pUnpackSizes;
                for(int num = 0;num < numOfUnpacksizes;num++){
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                    unpackSizes[num] = readHandle64Bit(curPos,pBuff,pFile);
                    cout<<"unpackSizes "<<unpackSizes[num]<<endl;
                    if(pCd->unpackSize == 0)
                        pCd->unpackSize = unpackSizes[num];
                }
            }/* if(curPos == CODER_UPACK_SIZE) */
            fread(buffer,1,1,pFile);
            curPos = int(buffer[0]);
            if(curPos == PROPERTY_CRC) { // 0x0a
                cout<<"CRC"<<endl;
                for(int num = 0;num < numFolders; num++){
                    fread(buffer,1,1,pFile);
                    curPos = int(buffer[0]);
                    streamsData.pFolders[num].digestDefined = (curPos==0x00) ? false:true;
                    cout<<"check bool "<<streamsData.pFolders[num].digestDefined<<endl;
                } /* end for */
                for(int num = 0; num < numFolders; num++){
                    fread(streamBuf.char_v,1,4,pFile);
                    cout<<"check crc "<<dec<<streamBuf.int_v<<endl; // it is little endian
                    pCd->crc = streamBuf.int_v;
                } /* end for */

            } /* end if(curPos == PROPERTY_CRC) */

        } /* end if(curPos == UNPACK_INFO) */
        if(curPos == SUBSTEAMS_INFO){

        }
//        cout<<"folder pointers "<<streamsData.pFolders[0].numCoders<<" "
//        <<streamsData.pFolders[0].pCoders[0].method<<" "
//        <<streamsData.pPackSizes[0]
//        <<endl;

        k++;
    } /* end while(k < tailOffset) */

    fseek(pFile,afterHeader,SEEK_SET);
    cout<<hex<<"afterHeader:"<<ftell(pFile)<<endl;
    for(int m = 0; m < streamsData.numFolders;m++){
        fseek(pFile,streamsData.packPos,SEEK_CUR);
//        fseek(pFile,0x8f800,SEEK_SET);
        cout<<streamsData.packPos<<" packPos:"<<ftell(pFile)<<endl;
        long props = streamsData.pFolders[m].pCoders[0].propertiesSize;
        cout<<"len "<<props<<endl;
        setDecoderProp(streamsData.pFolders[m].pCoders[0].pProp,props,pCd);//

//        fseek(pFile,streamsData.packPos,SEEK_SET);
        for(int idx = 0; idx < streamsData.numStreams;idx++){
            long cipherLen = streamsData.pPackSizes[idx];
            unsigned char *cipher = new unsigned char[cipherLen];
            fread(cipher,1,cipherLen,pFile);
            cout<<"len: "<<cipherLen<<" cipher:\n";
            for(int cl = 0; cl < cipherLen;cl++){
                cout<<(int)cipher[cl]<<" ";
            }
            cout<<endl;
            pCd->cipherLen = cipherLen;
//            pCd->cipher = (char *)cipher;
            pCd->cipher = cipher;
//            strcpy((char *)pCd->cipher,&cipher);
            cout<<"unpackSizes: "<<streamsData.pUnpackSizes[idx]<<endl;
        }
//        pCd->numCyclesPower = streamsData.pFolders[m].

    }
//    cout<<"folder pointers out "<<streamsData.packPos<<"\t"<<streamsData.numFolders
//    <<"\t"<<streamsData.pFolders[0].numCoders<<endl;

    int close = fclose(pFile);
    cout<<"file is close ? "<<close<<endl;// 成功返回0,否则 EOF(-1)
    return 1;
}

//
//int main(){
////    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA_Password.7z";
////    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA_FileName_Password.7z";
//    char path[] = "/Users/shuai/Desktop/filesfor7z/fmt_main1.7z";
//
//    CheckData cd;
//    CheckData *pCd;
//    pCd = &cd;
//    cout<<ReadByPath(path,pCd)<<endl;
//
//
//    if(pIv){
//        delete pIv;
//        pIv = NULL;
//    }
//    if(pSalt){
//        delete pSalt;
//        pSalt = NULL;
//    }
//    return 0;
//}