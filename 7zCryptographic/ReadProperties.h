//
// Created by shuai on 16/4/12.
//

#ifndef INC_7ZCRYPTOGRAPHIC_READPROPERTIES_H
#define INC_7ZCRYPTOGRAPHIC_READPROPERTIES_H

#include <iostream>
using namespace std;
struct Coders{
    long method;
    long numInstream;
    long numOutstream;
    long propertiesSize;
    unsigned char *pProp;
	Coders():propertiesSize(0){};
    ~Coders(){
        if(pProp){
            delete pProp;
            pProp = NULL;
        }
    }
};
struct Folders{
    int crc;
    long numCoders;
    Coders *pCoders;
    bool digestDefined;
    long numPackedStream;
    ~Folders(){
//        if(pCoders){
//            delete pCoders;
//            pCoders = NULL;
//        }
    }
};
struct StreamsData{
//    packInfo
    long packPos;
    long numStreams; //pPackSizes[numStream]
    long *pPackSizes;
    long *pUnpackSizes;
    long *pPackInfoCrcs;
//    unpackInfo
    long totalOut;
    long numFolders;
    Folders *pFolders;
    long dataStreamIdx;
//    subStreams
    ~StreamsData(){
//        if(pFolders){
//            delete pFolders;
//            pFolders = NULL;
//        }
//        if(pPackInfoCrcs){
//            delete pPackInfoCrcs;
//            pPackInfoCrcs = NULL;
//        }
        if(pPackSizes){
            delete pPackSizes;
            pPackSizes = NULL;
        }
        if(pUnpackSizes){
            delete pUnpackSizes;
            pUnpackSizes = NULL;
        }
    }
};
struct CheckData{
    long unpackSize;
    long numCyclesPower;
    long saltSize;
    std::string salt;
    long ivSize;
//    std::string iv;
    unsigned char * iv;
    // (what type ?)crc
    int crc;
    long cipherLen;
//    std::string cipher;
    unsigned char *cipher;
    char passwd[128];
	CheckData():unpackSize(0){};
//    ~CheckData(){
//        if(cipher){
//            delete cipher;
//            cipher = NULL;
//        }
//    }
};

//#include "Crc32.h"
#endif //INC_7ZCRYPTOGRAPHIC_READPROPERTIES_H
