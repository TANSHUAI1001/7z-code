#ifndef __7Z_ITEM_H
#define __7Z_ITEM_H
#include "MyBuffer.h"
#include "MyVector.h"
typedef unsigned int UInt32;
typedef UInt32 CNum;
const CNum kNumMax = 0x7FFFFFFF;
const CNum kNumNoIndex = 0xFFFFFFFF;
const UInt32 k_AES = 0x6F10701;
typedef unsigned long long int UInt64;
typedef UInt64 CMethodID;
struct CCoderInfo
{
	CMethodID MethodID;
	CByteBuffer Props;
	UInt32 NumStreams;

	bool IsSimpleCoder() const {return NumStreams == 1;}
};
struct CBond
{
	UInt32 PackIndex;
	UInt32 UnpackIndex;
};
struct CFolder
{
public:
	CObjArray2<CCoderInfo> Coders;
	CObjArray2<CBond> Bonds;
	CObjArray2<UInt32> PackStreams;

	CFolder() {}
	bool IsDecodingSupported() const {return Coders.Size() <= 32;}
	int Find_in_PackStreams(UInt32 packStream) const
	{
		FOR_VECTOR(i,PackStreams)
			if (PackStreams[i] == packStream)
				return i;
		return -1;
	}
	int Find_Bond_for_PackStream(UInt32 packStream) const
	{
		FOR_VECTOR(i,Bonds)
			if (Bonds[i].PackIndex == packStream)
				return i;
		return -1;
	}
	bool IsEncrypted() const
	{
		FOR_VECTOR(i,Coders)
			if(Coders[i].MethodID == k_AES)
				return true;
		return false;
	}
};
struct CUInt32DefVector
{
	CBoolVector Defs;
	CRecordVector<UInt32> Vals;
	void ClearAndSetSize(unsigned newSize)
	{
		Defs.ClearAndSetSize(newSize);
		Vals.ClearAndSetSize(newSize);
	}
	void Clear()
	{
		Defs.Clear();
		Vals.Clear();
	}
	void ReserveDown()
	{
		Defs.ReserveDown();
		Vals.ReserveDown();
	}
	bool ValidAndDefined(unsigned i) const
	{
		return i < Defs.Size() && Defs[i];
	}
};
struct CUInt64DefVector
{
	CBoolVector Defs;
	CRecordVector<UInt64> Vals;

	void Clear()
	{
		Defs.Clear();
		Vals.Clear();
	}
	void ReserveDown()
	{
		Defs.ReserveDown();
		Vals.ReserveDown();
	}
	bool GetItem(unsigned index, UInt64 &value) const
	{
		if (index < Defs.Size() && Defs[index])
		{
			value = Vals[index];
			return true;
		}
		value = 0;
		return false;
	}
	void SetItem(unsigned index, bool defined, UInt64 value);

	bool CheckSize(unsigned size) const { return Defs.Size() == size || Defs.Size() == 0; }
};
struct CFileItem
{
	UInt64 Size;
	UInt32 Attrib;
	UInt32 Crc;
	bool HasStream;
	bool IsDir;
	bool CrcDefined;
	bool AttribDefined;
	CFileItem():
		HasStream(true),
		IsDir(false),
		CrcDefined(false),
		AttribDefined(false)
	{}
	void SetAttrib(UInt32 attrib)
	{
		AttribDefined = true;
		Attrib = attrib;
	}
};
#endif