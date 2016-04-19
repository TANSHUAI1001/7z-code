#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char uint8;
typedef unsigned int uint32;

// iv_buffer.dat的内容：iv(16 Bytes) + '*' + cipher(最多131072 Bytes)
int main(int argc, char *agv[]){
	uint32 ivLen, cipherLen;
	char iv_buffer[16];	// 16 Bytes iv
	// AES-256 加解密一次的块大小：128 Bytes ，所以只读密文的128 Bytes
	char cipher_buffer[131072]; 
	
	FILE *pfd;
	pfd = fopen("iv_buffer.dat", "rb");
	assert(pfd!=NULL);
	ivLen = fread(iv_buffer, sizeof(char), 16, pfd);
	fseek(pfd, 1L, SEEK_CUR);	// 跳过'*'字符
	cipherLen = fread(cipher_buffer, sizeof(char), 131072, pfd);
	fclose(pfd);
	pfd = NULL;
	
	if(16 != ivLen || 131072 != cipherLen){
		printf("Read error?!\n");
	}else{
		// cipherLen = strlen(cipher_buffer); // 防止加密数据中有0x00
		for(int i=cipherLen-1; i>=0; i--){
			if(!cipher_buffer[i]) 
				cipherLen--;
			else
				break;
		}
		
		if(cipherLen < 16){
			printf("cipherLen = %d < 16Bytes !?", cipherLen);
			exit(-1);
		}
		
		printf("ivLen: %d, iv_buffer: ", ivLen);
		for(int i=0; i<ivLen; i++){
			printf("%.2x ", (uint8)iv_buffer[i]);
		}
		printf("\n");
		
		printf("cipherLen: %d, cipher: ", cipherLen);
		for(int i=0; i<cipherLen; i++){
			printf("%.2x", (uint8)cipher_buffer[i]);
		}
		printf("\n");
		
		// 写到文件中
		pfd = fopen("7zTask.txt", "w");
		assert(pfd!=NULL);
		for(int i=0; i<ivLen; i++){
			fprintf(pfd, "%.2x", (uint8)iv_buffer[i]);
		}
		fprintf(pfd, "*");
		// for(int i=0; i<cipherLen; i++){
		for(int i=0; i<16; i++){ // 只需写入16 Bytes 即AES解密的第一个块大小
			fprintf(pfd, "%.2x", (uint8)cipher_buffer[i]);
		}
		fclose(pfd);
		pfd = NULL;
	}
	return 0;
}