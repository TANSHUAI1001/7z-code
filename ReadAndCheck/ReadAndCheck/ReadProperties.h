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
	Coders():propertiesSize(0),pProp(NULL){};
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
	Folders():pCoders(NULL){}
    ~Folders(){
		pCoders = NULL;
       /* if(pCoders){
            delete pCoders;
            pCoders = NULL;
        }*/
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
	StreamsData():pFolders(NULL),pPackInfoCrcs(NULL),pUnpackSizes(NULL),pPackSizes(NULL){}
    ~StreamsData(){
		/*if(pFolders){
			delete pFolders;
			pFolders = NULL;
		}*/
		pFolders = NULL;
		if(pPackInfoCrcs){
			delete pPackInfoCrcs;
			pPackInfoCrcs = NULL;
		}
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
	bool aesCode;
    long *pUnpackSize; // ++
    long numCyclesPower;
    long saltSize;
    std::string salt;
    long ivSize;
    unsigned char * iv;
    int crc;
    long cipherLen;
    unsigned char *cipher;
    char passwd[128];

	bool lzmaCode;
	unsigned char *props;
	int propSize;

	CheckData():aesCode(false),lzmaCode(false),pUnpackSize(NULL),iv(NULL),cipher(NULL),props(NULL){}
	~CheckData(){
		if(pUnpackSize){
			delete pUnpackSize;
			pUnpackSize = NULL;
		}
		iv = NULL;
		cipher = NULL;
		props = NULL;
		/*if(iv) delete iv;
		if(cipher) delete cipher;
		if(props) delete props;*/
	}
};

//#include "Crc32.h"


#endif //INC_7ZCRYPTOGRAPHIC_READPROPERTIES_H
