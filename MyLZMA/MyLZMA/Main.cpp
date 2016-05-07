#include<stdio.h>
#include<stdlib.h>
#include"MyLzma.h"
int main(){	
	//if(LzmaCompress("C:/Users/shuai/Desktop/fmt_main1.7z","C:/Users/shuai/Desktop/fmt_main1.lzma"))
	//	printf("LzmaCompress OK \n");
	//else
	//	printf("LzmaCompress ERR \n");
	//if(LzmaUncompress("C:/Users/shuai/Desktop/out.dat","C:/Users/shuai/Desktop/out1001.dat"))
	if(MyLzmaUncompress("C:/Users/shuai/Desktop/LZMA_TEMP","C:/Users/shuai/Desktop/out1002.dat"))
		printf("LzmaUncompress OK \n");
	else
		printf("LzmaUncompress ERR \n");
	system("PAUSE");
	return 0;
}
