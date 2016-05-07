#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

size_t ConvertHexStrToInt(const char* hex_str,size_t length)
{
	size_t sum = 0;
	for(size_t i = 0;i < length; ++i)
	{
		int asc = (int)hex_str[i];
		size_t r1 =(asc & 0x40)?(asc&0x0F)+0x9:(asc & 0x0F);
		sum+=(r1*pow((double)16,int (length-i-1) ));
	}
	return sum;
}

uint8_t * ConvertToBinBuf(const char* hex_str,size_t *buf_size)
{
	if(!hex_str)
	{
		*buf_size = 0;
		return NULL;
	}
	//1.Å¼Êý¸ö
	size_t len = strlen(hex_str);
	assert(!(len%2));

	*buf_size = len/2;
	uint8_t* buf = (uint8_t*)malloc(*buf_size);

	for(size_t i = 0,j = 0; i<len; i+=2,++j)
	{
		uint8_t value = (uint8_t)ConvertHexStrToInt(hex_str+i,2);
		buf[j] = value;
	}
	return buf;
}

//int main(int argc, char *argv[])
//{
//	printf(".......................\n");
//
//	const char* str= "000000DD000000E00000015C000000E2";
//	//	uint8_t buf[]=
//	//	{
//	//		0x00,0x00,0x00,0xDD,
//	//		0x00,0x00,0x00,0xE0,
//	//		0x00,0x00,0x01,0x5C,
//	//		0x00,0x00,0x00,0xE2
//	//	};
//
//	FILE* file = fopen("E:/1.bin","wb");
//	assert(file);
//
//	size_t len = 0;
//	uint8_t* buf2 = ConvertToBinBuf(str,&len);
//	fwrite(buf2,1,len,file);
//	free(buf2);
//
//	fclose(file);
//
//	printf(".......................\n");
//	return 0;
//}
