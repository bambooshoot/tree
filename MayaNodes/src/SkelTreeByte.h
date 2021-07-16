#pragma once

#include <SkelTreeData.h>
#include <SkelChainOpBase.h>

NS_BEGIN

template<class T, class BYTECONV>
class ListByte
{
public:
	Uint byteSize(std::vector<T>& dataList) const
	{
		BYTECONV byteConv;
		Uint dataSize = dataList.size();
		for (auto& data : dataList)
			dataSize += byteConv.byteSize(data);

		return dataSize;
	}
};

#define LIST_BYTE_SIZE(T, DATASIZE, DATALIST) { \
ListByte<T, T##Byte> listConv; \
DATASIZE += listConv.byteSize(DATALIST); \
}

#define BYTE_SIZE(T, DATASIZE, DATAOBJ) { \
T##Byte conv;\
DATASIZE += conv.byteSize(DATAOBJ);\
}

class FrameDataByte
{
public:
	Uint byteSize(FrameData& frameData) const
	{
		return sizeof(frameData.q) + sizeof(frameData.xOffset);
	}
};

class OffsetFrameDataByte
{
public:
	Uint byteSize(OffsetFrameData& offsetFrameData) const
	{
		return sizeof(offsetFrameData.q) + sizeof(offsetFrameData.offset);
	}
};

class ChainDataByte
{
public:
	Uint byteSize(ChainData& chainData) const
	{
		Uint dataSize = 0;
		LIST_BYTE_SIZE(FrameData, dataSize, chainData.frameDataList)
		BYTE_SIZE(OffsetFrameData, dataSize, chainData.offsetFrameData)
		return dataSize;
	}
};

class DeformedMeshDataByte
{
public:
	Uint byteSize(DeformedMeshData& meshData) const
	{

	}
};

class FoliageDataByte
{
public:
	Uint byteSize(FoliageData& foliageData) const
	{

	}
};



class SkelTreeDataByte
{
public:
	Uint byteSize(SkelTreeData& treeData) const
	{
		Uint dataSize = 0;
		LIST_BYTE_SIZE(ChainData, dataSize, treeData.chainDataList)
		LIST_BYTE_SIZE(DeformedMeshData, dataSize, treeData.deformedDataList)
		LIST_BYTE_SIZE(FoliageData, dataSize, treeData.foliageDataList)

		dataSize += sizeof(Ushort);//Ushort trunkChainId
		dataSize += sizeof(float) * 6;//Box boundBox
		return dataSize;
	}
	void*			toByte(SkelTreeData& obj) const;
	SkelTreeData	toObj(void* pData) const;
};

NS_END