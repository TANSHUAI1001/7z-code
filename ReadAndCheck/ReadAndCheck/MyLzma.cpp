#include"MyLzma.h"
#include<stdio.h>
#include<stdlib.h>
#include <iostream>
using namespace std;
bool LzmaCompress(const char*scrfilename,const char*desfilename){

	FILE*fin=fopen(scrfilename,"rb");
	if(fin==NULL){
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	FILE*fout=fopen(desfilename,"wb");
	if(fout==NULL){
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize=ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize=saveinsize*1.1+1026*16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	unsigned char props[5]={0};
	size_t propsSize=5;
	size_t readlength=fread(inbuff,1,saveinsize,fin);
	if(readlength!=saveinsize){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res=LzmaCompress(outbuff,&saveoutsize,inbuff,saveinsize,
		props,&propsSize,
		5,
		1 << 24,
		3,
		0,
		2,
		32,
		1);
	if(res!=0){
		printf("LzmaCompressErr:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	int zero=0;
	fwrite(props,1,propsSize,fout);
	fwrite(&saveinsize,1,4,fout);
	fwrite(&zero,1,4,fout);
	fwrite(outbuff,1,saveoutsize,fout);

	fclose(fin);
	fclose(fout);
	return true;
}
bool LzmaUncompress(const char*scrfilename,const char*desfilename){
	FILE*fin=fopen(scrfilename,"rb");
	if(fin==NULL){
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	FILE*fout=fopen(desfilename,"wb");
	if(fout==NULL){
		printf("Open DesFile ERR:%s\n",desfilename);
		fclose(fin);
		return false;
	}

	fseek(fin,0,SEEK_END);
	size_t saveinsize=ftell(fin);
	fseek(fin,0,SEEK_SET);
	size_t saveoutsize=saveinsize*1.1+1026*16;
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	unsigned char props[5]={0};
	size_t propsSize=5;

	fread(props,1,5,fin);
	fread(&saveoutsize,1,4,fin);
	fseek(fin,4,SEEK_CUR);
	unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	size_t readlength=fread(inbuff,1,saveinsize-13,fin);
	if(readlength!=(saveinsize-13)){
		printf("read err\n");
		fclose(fin);
		fclose(fout);
		return false;
	}
	int	res=LzmaUncompress(outbuff,&saveoutsize,inbuff,&readlength,
		props,propsSize);
	if(res!=0){
		printf("LzmaUncompress:%d\n",res);
		fclose(fin);
		fclose(fout);
		return true;
	}
	fwrite(outbuff,1,saveoutsize,fout);
	fclose(fin);
	fclose(fout);
	return true;
}
//// MY LZMA compress 
//// 2016-5-5
bool MyLzmaUncompress(const char*scrfilename,unsigned char *&outbuff,size_t propsSize,
					  unsigned char *props,int inSize,int outSize){
	FILE*fin=fopen(scrfilename,"rb");
	if(fin==NULL){
		printf("Open ScrFile ERR:%s\n",scrfilename);
		return false;
	}
	/*FILE*fout=fopen(desfilename,"wb");
	if(fout==NULL){
	printf("Open DesFile ERR:%s\n",desfilename);
	fclose(fin);
	return false;
	}*/

	fseek(fin,0,SEEK_END);
	size_t saveinsize=ftell(fin);
	//size_t saveinsize = inSize;
	fseek(fin,0,SEEK_SET);
	/*size_t readlength = saveinsize - 1;*/
	size_t saveoutsize=saveinsize*1.1+1026*16;
	/*size_t propsSize=5;
	unsigned char props[5]={'\x5d','\x00','\x10','\x00','\x00'};*/
	unsigned char* inbuff=(unsigned char*)malloc(saveinsize);
	size_t readlength=fread(inbuff,1,inSize,fin);
	for (int xx = 0; xx < readlength; xx++)
	{
		cout<<hex<<(int)inbuff[xx]<<" ";
	}
	cout<<endl;
	saveoutsize = outSize;
	//unsigned char* outbuff=(unsigned char*)malloc(saveoutsize);
	outbuff=(unsigned char*)malloc(saveoutsize);
	int	res=LzmaUncompress(outbuff,&saveoutsize,inbuff,&readlength,
		props,propsSize);
	if(res!=0){
		printf("LzmaUncompress:%d\n",res);
		fclose(fin);
		//fclose(fout);
		return true;
	}
	//fwrite(outbuff,1,saveoutsize,fout);
	fclose(fin);
	//fclose(fout);
	return true;
}