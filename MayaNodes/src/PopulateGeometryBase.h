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

class PopulateGeometryBase
{
public:
	virtual uint vertexSize() const = 0;
	virtual uint indexSize() const = 0;
	virtual void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf)  = 0;
	virtual void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) = 0;
	virtual void populateGeometryIndex(MGeometry& data, const MRenderItem* item, const MString& renderItemName) = 0;

	virtual void prepare(skelTree::SkelTreeDataP pTreeData, const uint vertexOffsetId, const uint indexOffsetId)
	{
		_pTreeData = pTreeData;
		_vertexOffsetId = vertexOffsetId;
		_indexOffsetId = indexOffsetId;
	}

protected:
	uint _vertexOffsetId;
	uint _indexOffsetId;
	skelTree::SkelTreeDataP _pTreeData;

	uint _positionOffsetId() const
	{
		return _vertexOffsetId * 3;
	}

	uint _colorOffsetId() const
	{
		return _vertexOffsetId * 4;
	}
};
