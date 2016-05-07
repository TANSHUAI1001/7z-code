#ifndef __COMMENT_H
#define __COMMENT_H

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

#endif