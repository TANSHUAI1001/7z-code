//
// Created by shuai on 16/4/14.
//
#include <stdint.h>
extern uint8_t * ConvertToBinBuf(const char* hex_str,size_t *buf_size);
#include "ReadProperties.h"
#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <stdio.h>
#include <assert.h>
#ifdef _MSC_VER
#include <malloc.h>
#else
#include <mm_malloc.h>
#endif 
#include "Crc32.h"
#include <fstream>
#include "StdOutStream.h"
#include "StdInStream.h"
#include "MyBuffer.h"
#include "MyCom.h"
#include "MyString.h"
#include "IPassword.h"
#include "MyCom.h"
#include "IArchive.h"
#include "IPassword.h"
extern bool MyLzmaUncompress(const char*scrfilename,unsigned char*&outBuff,size_t propsSize,
							 unsigned char *props,int inSize,int outSize);
extern bool MyLzmaUncompress2(unsigned char *CompleteBuff,unsigned char *&outbuff,size_t propsSize,
							  unsigned char *props,int inSize,int outSize);

typedef unsigned long uint64;
typedef unsigned int uint32 ;
typedef unsigned char uint8;

using namespace std;
extern int ReadByPath(char *fp,CheckData *&pCd);
CheckData cd;
CheckData *pCd;

int init(char path[]){
	

#ifdef _MSC_VER
	//char path[] = "C:/Users/shuai/Desktop/fmt_main1.7z";
	//char path[] = "C:/Users/shuai/Desktop/LZMA_FileName_Password.7z";
#else
    char path[] = "/Users/shuai/Desktop/filesfor7z/Desktop0000.7z";
#endif
//    char path[] = "/Users/shuai/Desktop/filesfor7z/fmt_main1.7z";
//    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA2_FileName_Password.7z";

    pCd = &cd;
	/*char pPath[100];
	cout<<"input file path :\n";
	scanf("%s",pPath);
	cout<<ReadByPath(pPath,pCd)<<endl;
	*/
    return ReadByPath(path,pCd);

//    return 0;
}
// stringToUnicode UTF-8 -> UCS-2
void stringToUnicode(char *src, char *rst){
    int len = strlen((char *)src);
    int i = 0;
    for(i = 0; i < len; i++){
//        rst[i] = src[i];
//        rst[2*i + 1] = src[i];
//        rst[2*i + 0] = '\x00'; //在每个字节前面加 '\0' == '\x00' != '0x00'
        rst[2*i + 0] = src[i]; //it is little endian
        rst[2*i + 1] = '\x00';
    }
}

UString GetPassword(CStdOutStream *outStream)
{
	if (outStream)
	{
		*outStream << "\nEnter password"
#ifdef MY_DISABLE_ECHO
			" (will not be echoed)"
#endif
			":";
		outStream->Flush();
	}

#ifdef MY_DISABLE_ECHO

	HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
	bool wasChanged = false;
	DWORD mode = 0;
	if (console != INVALID_HANDLE_VALUE && console != 0)
		if (GetConsoleMode(console, &mode))
			wasChanged = (SetConsoleMode(console, mode & ~ENABLE_ECHO_INPUT) != 0);
	UString res = g_StdIn.ScanUStringUntilNewLine();
	if (wasChanged)
		SetConsoleMode(console, mode);
	if (outStream)
	{
		*outStream << endl;
		outStream->Flush();
	}
	return res;

#else

	return g_StdIn.ScanUStringUntilNewLine();

#endif
}

void getTextPassword(unsigned char *&pwd, uint32 &pwdLen){
    //char password[100];
    //printf("input password:\n");
    //scanf("%s",password);

    //pwdLen = strlen(password) * 2;
    //pwd = (char *)malloc((pwdLen + 1)*(sizeof(char)));
    //stringToUnicode(password,pwd);

	/*new input method*/
	CStdOutStream *g_StdStream = NULL;
	g_StdStream = &g_StdOut;
	BSTR *passwd = NULL;
	UString password;
	ICryptoGetTextPassword *myGetTextPassword;
	password = GetPassword(g_StdStream);
	size_t len;
	len = password.Len();
	pwdLen = len * 2;
	pwd = (unsigned char *)malloc((pwdLen + 1)*(sizeof(unsigned char)));
	CByteBuffer buffer(len * 2);
	for (size_t i = 0; i < len; i++)
	{
		wchar_t c = password[i];
		((Byte *)buffer)[i * 2] = (Byte)c;
		((Byte *)buffer)[i * 2 + 1] = (Byte)(c >> 8);
		((Byte *)pwd)[i * 2] = (Byte)c;
		((Byte *)pwd)[i * 2 + 1] = (Byte)(c >> 8);
	}
}
void calculateDigest(unsigned char *pwd, uint32 pwdLen, uint8 *key, uint32 keyLen,CheckData *pCd){ // keyLen = 32
    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    const uint32 NumCyclesPower = 19;
    const uint32 numRounds = (uint64)(1 << NumCyclesPower);	// NumCyclesPower的值是19，这个数字来源于Props的第一字节
    uint8 temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    char *saltChar = new char[pCd->saltSize];
    strcpy(saltChar,pCd->salt.c_str());
    for(uint32 round = 0; round < numRounds; round++){
//        SHA256_Update(&ctx, saltChar,pCd->saltSize); // SaltSize = 0, 第一句实际上什么也没做 ?! => saltSize >= 2;

        SHA256_Update(&ctx, pwd, pwdLen);
        SHA256_Update(&ctx, temp, 8);
        for(int i=0; i<8; i++)
            if(++(temp[i])!=0)
                break;
    }
    SHA256_Final(key, &ctx);
    delete saltChar;
}

int main(int argc, char **argv){
	/*for (int i = 0; i < argc;i++)
	{
		cout<<i<<"argv:"<<argv[i]<<endl;
	}*/
#ifdef _DEBUG //
	char path[] = "C:/util/util_Pi.7z";
	int Rf = init(path);
#else
	if(argc < 2){
		cout<<"Usage:\tReadAndCheck <file_name> "<<endl;
		return 0;
	}
	else if (argc > 2)
	{
		cout<<"Not correct use !"<<endl;
		cout<<"Usage:\tReadAndCheck <file_name> \n Totally take 1 parameter !"<<endl;
		return 0;
	}

	int Rf = init(argv[1]);
#endif // DEBUG

	
	if (!Rf)
	{
		cout<<"Can not open file !"<<endl;
		return 0;
	}
	if (!pCd->aesCode)
	{
		cout<<"Not support !"<<endl;
		return 0;
	}
    unsigned char *pwd;
    uint32 pwdLen;
    int i = 0;
    getTextPassword(pwd,pwdLen);
//    AesGenTables(); //step 0
    for(i = 0; i < pwdLen;i++){
        printf("%.2x ",pwd[i]);
    }
    printf("\n");

    uint32 keyLen = 32, offset = 2;
    uint8 key[32];
    calculateDigest(pwd,pwdLen,key,keyLen,pCd);

    printf("key:\n");
    for(i = 0; i < keyLen; i++){
        printf("%.2d ",key[i]);
    }
    printf("\n");
    uint8 *iv_arr = pCd->iv;
    printf("iv:\n");
    for(i = 0; i < 16; i++){
        cout<<hex<<int(iv_arr[i])<<" ";
    }
    printf("\n\n");

    int cipherLen = pCd->cipherLen;
    uint8  *cipher = pCd->cipher;
    for(i = 0; i < cipherLen; i++){
        cout<<hex<<int(pCd->cipher[i])<<" ";
    }
    printf("\n\n");

   /*
    * start 7z_fmt_plug.c
    */

    AES_KEY akey;
    if(AES_set_decrypt_key(key,256,&akey) < 0){
        fprintf(stderr,"AES_set_decrypt_key failed in crypt !\n");
    }
#ifdef _MSC_VER
	uint8 *out;
	out = (uint8 *)malloc(cipherLen);
	
#else
    uint8 out[cipherLen];
#endif
    AES_cbc_encrypt(cipher,out,cipherLen,&akey,iv_arr,AES_DECRYPT);
	
    for(i = 0; i < cipherLen; i++){
        cout<<hex<<int(out[i])<<" ";
    }
    printf("\n");
	// 
	/*printf("write to file !\n");
	char desfilename[] = "C:/Users/shuai/Desktop/LZMA_TEMP";
	FILE*fout=fopen(desfilename,"wb");
	if(fout==NULL) printf("Open DesFile ERR:%s\n",desfilename);
	fwrite(out,1,cipherLen,fout);
	fclose(fout);*/

    int margin,nbytes,index;
	/*cout<<"++++++++++++++++++++"<<pCd->pUnpackSize[0]<<endl;
	cout<<"--------------------"<<cipherLen<<endl;
	cout<<"++++++++++++++++++++"<<pCd->pUnpackSize[1]<<endl;*/
    margin = nbytes = cipherLen - pCd->pUnpackSize[0];
    index = cipherLen - 1;
    while(nbytes > 0){
        if(out[index] != 0){
            cout<<"wrong password !"<<endl;
        }
        nbytes--;
        index--;
    }
	/*  if(margin > 7){
	cout<<"right password !"<<endl;
	}*/
    CRC32_t crc;
    union {
        unsigned char crcc[4];
        unsigned int crci;
    } _crc_out;
    unsigned char * crc_out = _crc_out.crcc;
    unsigned int crcc;
	/*for (int i = pCd->unpackSize; i > 0; i--)
	{
	checkOut[i] = out[i + margin];
	}*/
	uint8 *checkOut;
	int checkOutLength;
	if (pCd->lzmaCode)
	//if(0)
	{
		checkOutLength = pCd->pUnpackSize[1];
		/*int res = MyLzmaUncompress("C:/Users/shuai/Desktop/LZMA_TEMP",checkOut,pCd->propSize,pCd->props,pCd->pUnpackSize[0],pCd->pUnpackSize[1]);*/
		int res = MyLzmaUncompress2(out,checkOut,pCd->propSize,pCd->props,pCd->pUnpackSize[0],pCd->pUnpackSize[1]);
		if(!res){
			cout<<"Not support !"<<endl;
			return 0;
		}
		for (int kk = 0; kk < checkOutLength;kk++)
		{
			cout<<(int)checkOut[kk]<<" ";
		}
		cout<<endl;
	}
	else
	{
		
		checkOut = (uint8 *)malloc(pCd->pUnpackSize[0]);
		checkOutLength = pCd->pUnpackSize[0];
		for (int i = 0; i < pCd->pUnpackSize[0];i++)
		{
			checkOut[i] = out[i];
			cout<<(int)checkOut[i]<<" ";
		}
	}
	
	cout<<endl;
    CRC32_Init(&crc);
    CRC32_Update(&crc,checkOut,checkOutLength);
    CRC32_Final(crc_out,crc);
    cout<<"pCd->unpackSize "<<pCd->pUnpackSize[0]<<endl;
    crcc = _crc_out.crci;
    cout<<hex<<"crc "<<crcc<<" pcd->crc "<<pCd->crc<<endl;
	bool com = (crcc == pCd->crc);
    cout<<"compare CRC :"<<com<<endl;
	if (com)
	{
		cout<<"Right password !"<<endl;
	}
	else{
		cout<<"Wrong password !"<<endl;
	}
    /*
     * end 7z_fmt_plug
     */

#ifdef _MSC_VER
	free(out);
#endif
    delete iv_arr;
    delete cipher;
    delete pwd;
#ifdef _DEBUG
	system("pause");
#endif // DEBUG
    return 0;
}