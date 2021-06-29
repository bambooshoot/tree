#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryDeformedPoints : public PopulateGeometryBase
{
public:
	uint vertexSize() const
	{
		return  _pTreeData->deformedPointNum();
	}
	uint indexSize() const
	{
		return  _pTreeData->deformedPointNum();
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = _positionOffsetId();
		for (auto& deformedData : _pTreeData->deformedDataList)
			for (auto& p : _pTreeData->pointsList[deformedData.pointsId].finalPositions()) {
				buf[idx++] = p.x;
				buf[idx++] = p.y;
				buf[idx++] = p.z;
			}
	}

	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = _colorOffsetId();
		for (auto& deformedData : _pTreeData->deformedDataList)
			for (auto& w : deformedData.wList) {
				buf[idx++] = w.w[0];
				buf[idx++] = w.w[1];
				buf[idx++] = 0.0f;
				buf[idx++] = 1.0f;
			}
	}

	void populateGeometryIndex(MGeometry& data, const MRenderItem* item, const MString& renderItemName) override
	{
		uint vertexDataSize = indexSize();
		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == renderItemName)
		{
			int numIndex = vertexDataSize;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex, true);

			for (int i = 0; i < numIndex; ++i)
				indices[i] = i + _indexOffsetId;

			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}
};