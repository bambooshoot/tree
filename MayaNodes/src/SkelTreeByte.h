#pragma once

#include <SkelTreeData.h>
#include <SkelChainOpBase.h>

NS_BEGIN

template<typename T, typename BYTECONV>
Uint listByteSize(std::vector<T>& dataList)
{
	Uint dataSize = (Uint)dataList.size();
	Uint offset = sizeof(Uint);

	for (Uint i = 0; i < dataSize; ++i)
		offset += BYTECONV::byteSize(dataList[i]);

	return offset;
};

#define LIST_BYTE_SIZE(T, DATALIST) listByteSize<T, T##Byte>(DATALIST);

template<typename T>
Uint byteFill(T& obj, Char* data)
{
	*(T*)data = obj;
	T chkValue = *(T*)data;
	return sizeof(T);
}

template<>
Uint byteFill(Vec& v, Char* data)
{
	const Uint dataSize = sizeof(Float) * 3;
	byteFill<Float>(v.x, data);
	byteFill<Float>(v.y, data + sizeof(Float));
	byteFill<Float>(v.z, data + sizeof(Float) * 2);
	return dataSize;
}

template<>
Uint byteFill(Quat& q, Char* data)
{
	Uint offset = byteFill<Float>(q.r, data);
	offset += byteFill<Vec>(q.v, data + offset);
	return offset;
}

template<>
Uint byteFill(Box& box, Char* data)
{
	Uint offset = byteFill<Vec>(box.min, data);
	offset += byteFill<Vec>(box.max, data + offset);
	return offset;
}

template<typename T, Int DIM>
Uint byteArrayFill(T* obj, Char* data)
{
	Uint offset = 0;
	for (Uint i = 0; i < DIM; ++i)
		offset += byteFill<T>(obj[i], data + offset);

	return offset;
}

template<typename T, typename BYTECONV>
Uint listByteFill(std::vector<T> & dataList, Char* data)
{
	Uint dataSize = (Uint)dataList.size();
	Uint offset = byteFill<Uint>(dataSize, data);

	for (Uint i = 0; i < dataSize; ++i) {
		offset += BYTECONV::toByte(dataList[i], data + offset);
	}

	return offset;
};

#define LIST_BYTE_FILL(T, DATALIST, PDATA) listByteFill<T, T##Byte>(DATALIST, PDATA);

template<typename T>
Uint objFill(T& obj, Char* data)
{
	obj = *(T*)data;
	return sizeof(T);
}

template<>
Uint objFill(Vec& v, Char* data)
{
	const Uint dataSize = sizeof(Float) * 3;
	objFill<Float>(v.x, data);
	objFill<Float>(v.y, data + sizeof(Float));
	objFill<Float>(v.z, data + sizeof(Float) * 2);
	return dataSize;
}

template<>
Uint objFill(Quat& q, Char* data)
{
	Uint offset = objFill<Float>(q.r, data);
	offset += objFill<Vec>(q.v, data + offset);
	return offset;
}

template<>
Uint objFill(Box& box, Char* data)
{
	Uint offset = objFill<Vec>(box.min, data);
	offset += objFill<Vec>(box.max, data + offset);
	return offset;
}

template<typename T, Int DIM>
Uint objArrayFill(T* obj, Char* data)
{
	Uint offset = 0;
	for (Uint i = 0; i < DIM; ++i)
		offset += objFill<T>(obj[i], data + offset);

	return offset;
}

template<typename T, typename BYTECONV>
Uint listObjFill(std::vector<T>& dataList, Char* data)
{
	Uint dataSize;
	Uint offset = objFill<Uint>(dataSize, data);
	
	dataList.clear();
	dataList.reserve(dataSize);

	T eltValue;
	for (Uint i = 0; i < dataSize; ++i) {
		offset += BYTECONV::toObj(eltValue, data + offset);
		dataList.emplace_back(eltValue);
	}

	return offset;
};

#define LIST_OBJ_FILL(T, DATALIST, PDATA) listObjFill<T, T##Byte>(DATALIST, PDATA);

struct FrameDataByte {
	static Uint byteSize(FrameData& frameData)
	{
		return sizeof(Float) * 4 + sizeof(Float);
	}
	static Uint toByte(FrameData& frameData, Char* data)
	{
		Uint offset = byteFill<Float>(frameData.xOffset, data);
		offset += byteFill<Quat>(frameData.q, data + offset);
		return offset;
	}
	static Uint toObj(FrameData& frameData, Char* data)
	{
		Uint offset = objFill<Float>(frameData.xOffset, data);
		offset += objFill<Quat>(frameData.q, data + offset);
		return offset;
	}
};

struct OffsetFrameDataByte {
	static Uint byteSize(OffsetFrameData& offsetFrameData)
	{
		return sizeof(Float) * 4 + sizeof(Float) * 3;
	}
	static Uint toByte(OffsetFrameData& offsetFrameData, Char* data)
	{
		Uint offset = byteFill<Vec>(offsetFrameData.offset, data);
		offset += byteFill<Quat>(offsetFrameData.q, data + offset);
		return offset;
	}
	static Uint toObj(OffsetFrameData& offsetFrameData, Char* data)
	{
		Uint offset = objFill<Vec>(offsetFrameData.offset, data);
		offset += objFill<Quat>(offsetFrameData.q, data + offset);
		return offset;
	}
};

struct ChainDataByte {
	static Uint byteSize(ChainData& chainData)
	{
		Uint dataSize = sizeof(Ushort);
		dataSize += OffsetFrameDataByte::byteSize(chainData.offsetFrameData);
		dataSize += LIST_BYTE_SIZE(FrameData, chainData.frameDataList)
		return dataSize;
	}
	static Uint toByte(ChainData& chainData, Char* data)
	{
		Uint offset = byteFill<Ushort>(chainData.trunkFrameId, data);

		offset += OffsetFrameDataByte::toByte(chainData.offsetFrameData, data + offset);
		offset += LIST_BYTE_FILL(FrameData, chainData.frameDataList, data + offset)
		return offset;
	}
	static Uint toObj(ChainData& chainData, Char* data)
	{
		Uint offset = objFill<Ushort>(chainData.trunkFrameId, data);

		offset += OffsetFrameDataByte::toObj(chainData.offsetFrameData, data + offset);
		offset += LIST_OBJ_FILL(FrameData, chainData.frameDataList, data + offset)
		return offset;
	}
};

struct DeformedMeshWeightByte {
	static Uint byteSize(DeformedMeshWeight& deformedMeshWeight)
	{
		return sizeof(Uint) + (sizeof(Ushort) + sizeof(Float)) * DEFORMED_WEIGHT_NUM;
	}
	static Uint toByte(DeformedMeshWeight& deformedMeshWeight, Char* data)
	{
		Uint offset = byteFill<Uint>(deformedMeshWeight.pId, data);

		offset += byteArrayFill<Ushort, DEFORMED_WEIGHT_NUM>(deformedMeshWeight.w.id, data + offset);
		offset += byteArrayFill<Float, DEFORMED_WEIGHT_NUM>(deformedMeshWeight.w.w, data + offset);
		return offset;
	}
	static Uint toObj(DeformedMeshWeight& deformedMeshWeight, Char* data)
	{
		Uint offset = objFill<Uint>(deformedMeshWeight.pId, data);

		offset += objArrayFill<Ushort, DEFORMED_WEIGHT_NUM>(deformedMeshWeight.w.id, data + offset);
		offset += objArrayFill<Float, DEFORMED_WEIGHT_NUM>(deformedMeshWeight.w.w, data + offset);
		return offset;
	}
};

struct DeformedMeshDataByte {
	static Uint byteSize(DeformedMeshData& deformedMeshData)
	{
		Uint dataSize = sizeof(Ushort) * 2;
		dataSize += LIST_BYTE_SIZE(DeformedMeshWeight, deformedMeshData.wList)
		return dataSize;
	}
	static Uint toByte(DeformedMeshData& deformedMeshData, Char* data)
	{
		Uint dataSize = byteFill<Ushort>(deformedMeshData.chainId, data);
		dataSize += byteFill<Ushort>(deformedMeshData.pointsId, data + dataSize);

		dataSize += LIST_BYTE_FILL(DeformedMeshWeight, deformedMeshData.wList, data + dataSize)
		return dataSize;
	}
	static Uint toObj(DeformedMeshData& deformedMeshData, Char* data)
	{
		Uint dataSize = objFill<Ushort>(deformedMeshData.chainId, data);
		dataSize += objFill<Ushort>(deformedMeshData.pointsId, data + dataSize);

		dataSize += LIST_OBJ_FILL(DeformedMeshWeight, deformedMeshData.wList, data + dataSize)
		return dataSize;
	}
};

struct FoliageDataByte {
	static Uint byteSize(FoliageData& foliageData)
	{
		return sizeof(Ushort)
			+ sizeof(Ushort) + sizeof(Uint) * 3 + sizeof(Float) * 2
			+ sizeof(Float)
			+ sizeof(Float) * 4;
	}
	static Uint toByte(FoliageData& foliageData, Char* data)
	{
		Uint dataSize = byteFill<Ushort>(foliageData.pointsId, data);
		dataSize += byteFill<Ushort>(foliageData.attachPoint.pointsId, data + dataSize);

		dataSize += byteArrayFill<Uint, 3>(foliageData.attachPoint.vid, data + dataSize);
		dataSize += byteArrayFill<Float, 2>(foliageData.attachPoint.w, data + dataSize);

		dataSize += byteFill<Float>(foliageData.scale, data + dataSize);
		dataSize += byteFill<Quat>(foliageData.q, data + dataSize);
		return dataSize;
	}
	static Uint toObj(FoliageData& foliageData, Char* data)
	{
		Uint dataSize = objFill<Ushort>(foliageData.pointsId, data);
		dataSize += objFill<Ushort>(foliageData.attachPoint.pointsId, data + dataSize);

		dataSize += objArrayFill<Uint, 3>(foliageData.attachPoint.vid, data + dataSize);
		dataSize += objArrayFill<Float, 2>(foliageData.attachPoint.w, data + dataSize);

		dataSize += objFill<Float>(foliageData.scale, data + dataSize);
		dataSize += objFill<Quat>(foliageData.q, data + dataSize);
		return dataSize;
	}
};

struct SkelTreeDataByte
{
	static Uint byteSize(SkelTreeData& treeData)
	{
		Uint dataSize = LIST_BYTE_SIZE(ChainData, treeData.chainDataList)
		dataSize += LIST_BYTE_SIZE(DeformedMeshData, treeData.deformedDataList)
		dataSize += LIST_BYTE_SIZE(FoliageData, treeData.foliageDataList)

		dataSize += sizeof(Ushort);//Ushort trunkChainId
		dataSize += sizeof(Float) * 3 * 2;//Box boundBox
		return dataSize;
	}
	static Uint toByte(SkelTreeData& treeData, Char* data)
	{
		Uint dataSize = LIST_BYTE_FILL(ChainData, treeData.chainDataList, data)
		dataSize += LIST_BYTE_FILL(DeformedMeshData, treeData.deformedDataList, data + dataSize)
		dataSize += LIST_BYTE_FILL(FoliageData, treeData.foliageDataList, data + dataSize)

		dataSize += byteFill<Ushort>(treeData.trunkChainId, data + dataSize);
		dataSize += byteFill<Box>(treeData.boundBox, data + dataSize);
		return dataSize;
	}
	static Uint toObj(SkelTreeData & treeData, Char* data)
	{
		Uint dataSize = LIST_OBJ_FILL(ChainData, treeData.chainDataList, data)
		dataSize += LIST_OBJ_FILL(DeformedMeshData, treeData.deformedDataList, data + dataSize)
		dataSize += LIST_OBJ_FILL(FoliageData, treeData.foliageDataList, data + dataSize)

		dataSize += objFill<Ushort>(treeData.trunkChainId, data + dataSize);
		dataSize += objFill<Box>(treeData.boundBox, data + dataSize);
		return dataSize;
	}
};

struct AniOpDataByte
{
	static Uint byteSize(AniOpData& opData)
	{
		return sizeof(Float) * 3 + sizeof(Float) * 11;
	}
	static Uint toByte(AniOpData& opData, Char* data)
	{
		Uint dataSize = byteFill<Vec>(opData.windDirection, data);
		dataSize += byteArrayFill<Float,4>(opData.noiseTrunk, data + dataSize);
		dataSize += byteArrayFill<Float,3>(opData.noiseBranch, data + dataSize);
		dataSize += byteArrayFill<Float,4>(opData.noiseFoliage, data + dataSize);
		return dataSize;
	}
	static Uint toObj(AniOpData& opData, Char* data)
	{
		Uint dataSize = objFill<Vec>(opData.windDirection, data);
		dataSize += objArrayFill<Float, 4>(opData.noiseTrunk, data + dataSize);
		dataSize += objArrayFill<Float, 3>(opData.noiseBranch, data + dataSize);
		dataSize += objArrayFill<Float, 4>(opData.noiseFoliage, data + dataSize);
		return dataSize;
	}
};

struct SkelTreeFileByte
{
	static Uint byteSize(SkelTreeData& treeData, AniOpData& opData)
	{
		return SkelTreeDataByte::byteSize(treeData) + AniOpDataByte::byteSize(opData);
	}
	static Uint toByte(SkelTreeData& treeData, AniOpData& opData, Char* data)
	{
		Uint dataSize = SkelTreeDataByte::toByte(treeData, data);
		dataSize += AniOpDataByte::toByte(opData, data + dataSize);
		return dataSize;
	}
	static Uint toObj(SkelTreeData& treeData, AniOpData& opData, Char* data)
	{
		Uint dataSize = SkelTreeDataByte::toObj(treeData, data);
		dataSize += AniOpDataByte::toObj(opData, data + dataSize);
		return dataSize;
	}
};

NS_END