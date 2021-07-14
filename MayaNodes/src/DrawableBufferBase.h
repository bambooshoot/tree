#pragma once

#include <memory>
#include <map>

#include <maya/MIntArray.h>
#include <maya/MHWGeometry.h>

#include <SkelTree.h>

struct PopulateGeometryData
{
	float					chainAxisScale;
	std::vector<MIntArray>	triangleVtx;
};

struct DrawableBufferParam
{
	std::string					name;
	skelTree::SkelTreeDataP		pTreeData;
	skelTree::SkelTreeP			pTree;
	PopulateGeometryData*		pPopGeoData;
};

using namespace MHWRender;

class DrawableBufferBase
{
public:
	virtual ~DrawableBufferBase() 
	{
		_clear();
	};
	void setParam(DrawableBufferParam& param)
	{
		_param = param;
	}
	void build(MVertexBufferArray & refBufArray, MIndexBuffer*& idxBuf)
	{
		_createVertexBuffer(MGeometry::kPosition, 3, &DrawableBufferBase::fillPositions, refBufArray);
		
		if(hasNormal())
			_createVertexBuffer(MGeometry::kNormal, 3, &DrawableBufferBase::fillNormals, refBufArray);
		
		if(hasColor())
			_createVertexBuffer(MGeometry::kColor, 4, &DrawableBufferBase::fillColors, refBufArray);

		_createIndexBuffer(idxBuf);
	}
	virtual bool hasNormal() const = 0;
	virtual bool hasColor() const = 0;
	virtual bool hasInstance() const = 0;
	virtual void instanceMatrices(MMatrixArray& matArray) const {};

protected:
	using IdxBufPtr = std::unique_ptr<MIndexBuffer>;
	typedef void (DrawableBufferBase::* FillVtxDataFunc)(float * data);

	DrawableBufferParam			_param;

	virtual uint indexNum() const = 0;
	virtual uint vertexNum() const = 0;
	virtual void fillPositions(float* data) = 0;
	virtual void fillColors(float* data) {};
	virtual void fillNormals(float* data) {};
	virtual void fillIndices(uint* data) = 0;

	static std::map<MGeometry::Semantic, MString> sSematicName;

	void _createVertexBuffer(
		MGeometry::Semantic sematic,
		uint fltSize,
		FillVtxDataFunc pFunc,
		MVertexBufferArray& refBufArray
		) {
		
		const MVertexBufferDescriptor vbDesc(MString(_param.name.c_str()),
			sematic, MGeometry::kFloat, fltSize);

		uint elementNum = vertexNum();

		MVertexBuffer* fBuffer = new MVertexBuffer(vbDesc);
		float * data = (float*)fBuffer->acquire(elementNum, true);
		
		(this->*pFunc)(data);

		fBuffer->commit(data);

		refBufArray.addBuffer(DrawableBufferBase::sSematicName[sematic], fBuffer);
	}

	void _createIndexBuffer(MIndexBuffer*& idxBuf) {
		idxBuf = new MIndexBuffer(MGeometry::kUnsignedInt32);
		uint idxNum = indexNum();
		uint* data = (uint*)idxBuf->acquire(idxNum, true);

		fillIndices(data);

		idxBuf->commit(data);
	}

	void _clear() {
	}
};

using DrawableBufferPtr = std::unique_ptr<DrawableBufferBase>;
#define DrawableBufferMakeUnique(T) std::make_unique<DrawableBufferBase>(T)