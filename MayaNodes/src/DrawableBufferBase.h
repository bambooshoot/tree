#pragma once

#include <memory>
#include <map>

#include <maya/MIntArray.h>
#include <maya/MHWGeometry.h>

#include <SkelTree.h>
#include <SkelChainOpBase.h>

struct PopulateGeometryData
{
	float	chainAxisScale;
	int		segmentNum;
	float	noiseGraphScale;
};

struct DrawableBufferParam
{
	std::string					name;
	skelTree::SkelTreeDataP		pTreeData;
	skelTree::SkelTreeP			pTree;
	std::vector<MIntArray>*		pTriangleVtx;
	PopulateGeometryData*		pPopGeoData;
	skelTree::AniOpData			aniOpData;
};

using namespace MHWRender;

class DrawableBufferBase
{
public:
	virtual ~DrawableBufferBase() 
	{
		_clear();
	};
	void build(MVertexBufferArray & refBufArray, MIndexBuffer*& idxBuf, DrawableBufferParam& param)
	{
		_createVertexBuffer(MGeometry::kPosition, 3, &DrawableBufferBase::fillPositions, refBufArray, param, 0);
		
		if(hasNormal())
			_createVertexBuffer(MGeometry::kNormal, 3, &DrawableBufferBase::fillNormals, refBufArray, param, 1);
		
		if(hasColor())
			_createVertexBuffer(MGeometry::kColor, 4, &DrawableBufferBase::fillColors, refBufArray, param, 2);

		_createIndexBuffer(idxBuf, param);
	}
	virtual bool hasNormal() const = 0;
	virtual bool hasColor() const = 0;
	virtual bool hasInstance() const = 0;
	virtual void instanceMatrices(MMatrixArray& matArray, DrawableBufferParam& param) const {};

protected:
	using VtxBufPtr = std::unique_ptr<MVertexBuffer>;
	using IdxBufPtr = std::unique_ptr<MIndexBuffer>;
	typedef void (DrawableBufferBase::* FillVtxDataFunc)(float * data, DrawableBufferParam& param);

	virtual uint indexNum(DrawableBufferParam& param) const = 0;
	virtual uint vertexNum(DrawableBufferParam& param) const = 0;
	virtual void fillPositions(float* data, DrawableBufferParam& param) = 0;
	virtual void fillColors(float* data, DrawableBufferParam& param) {};
	virtual void fillNormals(float* data, DrawableBufferParam& param) {};
	virtual void fillIndices(uint* data, DrawableBufferParam& param) = 0;

	static std::map<MGeometry::Semantic, MString> sSematicName;

	VtxBufPtr vtxBuf[3];

	void _createVertexBuffer(
		MGeometry::Semantic sematic,
		uint fltSize,
		FillVtxDataFunc pFunc,
		MVertexBufferArray& refBufArray,
		DrawableBufferParam& param,
		uint bufIdx
		) {
		
		const MVertexBufferDescriptor vbDesc("", sematic, MGeometry::kFloat, fltSize);

		uint elementNum = vertexNum(param);

		vtxBuf[bufIdx] = MAKE_UNIQUE<MVertexBuffer>(vbDesc);
		float * data = (float*)vtxBuf[bufIdx]->acquire(elementNum, true);
		
		(this->*pFunc)(data, param);

		vtxBuf[bufIdx]->commit(data);

		refBufArray.addBuffer(DrawableBufferBase::sSematicName[sematic], vtxBuf[bufIdx].get());

		data = nullptr;
	}

	void _createIndexBuffer(MIndexBuffer*& idxBuf, DrawableBufferParam& param) {
		idxBuf = new MIndexBuffer(MGeometry::kUnsignedInt32);
		uint idxNum = indexNum(param);
		uint* data = (uint*)idxBuf->acquire(idxNum, true);

		fillIndices(data, param);

		idxBuf->commit(data);

		data = nullptr;
	}

	void _clear() {
	}
};

using DrawableBufferPtr = std::unique_ptr<DrawableBufferBase>;
#define DrawableBufferMakeUnique(T) std::make_unique<DrawableBufferBase>(T)