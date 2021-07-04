#pragma once

#include <maya/MString.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MShaderManager.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>

#include <SkelTree.h>

struct PopulateGeometryData
{
	float					chainAxisScale;
	std::vector<MIntArray>	triangleVtx;
};

class PopulateGeometryBase
{
public:
	virtual uint vertexSize() const = 0;
	virtual uint indexSize() const = 0;
	virtual void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf)  = 0;
	virtual void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) = 0;
	virtual void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) = 0;

	virtual void prepare(skelTree::SkelTreeDataP pTreeData, skelTree::SkelTreeP pTree, PopulateGeometryData* pPopGeoData, const uint vertexOffsetId)
	{
		_pTreeData = pTreeData;
		_pTree = pTree;
		_pPopGeoData = pPopGeoData;
		_vertexOffsetId = vertexOffsetId;
	}

	void populateGeometryIndex(MGeometry& data, const MRenderItem* item, const MString& renderItemName)
	{
		uint vertexDataSize = indexSize();
		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == renderItemName)
		{
			int numIndex = vertexDataSize;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex, true);

			_fillIndex(indices);

			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}

protected:
	uint					_vertexOffsetId;
	skelTree::SkelTreeDataP _pTreeData;
	skelTree::SkelTreeP		_pTree;
	PopulateGeometryData* _pPopGeoData;

	uint _positionOffsetId() const
	{
		return _vertexOffsetId * 3;
	}

	uint _colorOffsetId() const
	{
		return _vertexOffsetId * 4;
	}

	virtual void _fillIndex(unsigned int* indices) = 0;
};
