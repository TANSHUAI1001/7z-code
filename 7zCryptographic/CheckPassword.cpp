//
// Created by shuai on 16/4/8.
//
//#include <cryptopp>
#include "ReadProperties.h"
#include "Formats.h"
#include <openssl/sha.h>
#include <openssl/aes.h>
using namespace std;
extern int ReadByPath(char *fp,CheckData *pCd);
extern int ivSize,saltSize,numCyclesPower;
extern unsigned char * pIv;
extern unsigned char * pSalt;

static  char (*saved_key)[PLAINTEXT_LENGTH + 1];
static int *cracked;
static int validFolder(unsigned char *data){
    return 0;
}
static int sevenZipDecrypt(unsigned char *derived_key, unsigned char *data){
    unsigned char out[cur_salt->length];
    AES_KEY akey;
    unsigned char iv[16];
    union{
        unsigned char crcc[4];
        unsigned int crci;
    } _crc_out;
    unsigned char *crc_out = _crc_out.crcc;
    unsigned int ccrc;
    CRC32_t crc;
    int i,nbytes,margin;
    memcpy(iv,cur_salt->iv,16);
    if(AES_set_decrypt_key(derived_key,256,&akey) < 0){
        fprintf(stderr,"AES_set_decrypt_key failed in crypt !\n");
    }
    AES_cbc_encrypt(cur_salt->data,out,cur_salt->length,&akey,iv,AES_DECRYPT);
    margin = nbytes = cur_salt->length - cur_salt->unpacksize;
    i = cur_salt->length - 1;
    while (nbytes > 0){
        if(out[i] != 0){
            return -1;
        }
        nbytes--;
        i--;
    }
    if(margin > 7){
        return 0;
    }
    CRC32_Init(&crc);
    CRC32_Update(&crc,out,cur_salt->unpacksize);
    CRC32_Final(crc_out,crc);
    ccrc = _crc_out.crci;
    if(ccrc == cur_salt->crc){
        return 0;
    }
    if(validFolder(out)){
        printf("validFolder check !\n");
        return 0;
    }
    return -1;
}
void sevenZipKdf(UTF8 *passowrd, unsigned char *master){
    int len;
    long long rounds = (long long) 1 << cur_salt->NumCyclesPower;
    long long round;
    UTF16 buffer[PLAINTEXT_LENGTH + 1];
    SHA256_CTX sha;
    len = enc_to_utf16(buffer,PLAINTEXT_LENGTH,passowrd,strlen((char*)passowrd));
    if(len <= 0){
        passowrd[-len] = 0;
        len = strlen16(buffer);
    }
    len *= 2;
    SHA256_Init(&sha);
    for(round = 0; round < rounds;round++){
        SHA256_Update(&sha,(char*)buffer,len);
        SHA256_Update(&sha,(char*)&round,8);
    }
    SHA256_Final(master,&sha);

}
static int cryptAll(int *pcount){
    int count = *pcount;
    int index = 0;
    {
        unsigned  char master[32];
        sevenZipKdf((unsigned char *)saved_key[index],master);
        if(sevenZipDecrypt(master,cur_salt->data))
            cracked[index] = 1;
        else
            cracked[index] = 0;
    }
    return count;
}
bool CheckPassword(CheckData *pCd){

    return true;
}
int main(){
    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA_FileName_Password.7z";
//    char path[] = "/Users/shuai/Desktop/filesfor7z/LZMA2_FileName_Password.7z";
    CheckData cd;
    CheckData *pCd;
    pCd = &cd;
    cout<<ReadByPath(path,pCd)<<endl;
    pCd->numCyclesPower = numCyclesPower;
    pCd->ivSize = ivSize;
    pCd->iv = *pIv;
    pCd->saltSize = saltSize;
    pCd->salt = *pSalt;
    strcpy(pCd->passwd,"123");

    custom_salt salt;
    salt.NumCyclesPower = pCd->numCyclesPower;
    salt.crc = pCd->crc;
    salt.ivSize = pCd->ivSize;
    salt.SaltSize = pCd->saltSize;
//    cur_salt->iv = pCd->iv;
    strcpy((char*)salt.iv,pCd->iv.c_str());
    strcpy((char*)salt.salt,pCd->salt.c_str());
    salt.length = pCd->cipherLen;
    strcpy((char*)salt.data,pCd->cipher.c_str());
    salt.unpacksize = pCd->unpackSize;
    salt.type = 0;
    cur_salt = &salt;
    int count = 0;
    int *pInt;
    pInt = &count;
    cryptAll(pInt);
    free(saved_key[0]);



    if(pIv){
        delete pIv;
        pIv = NULL;
    }
    if(pSalt){
        delete pSalt;
        pSalt = NULL;
    }
    return 0;
}