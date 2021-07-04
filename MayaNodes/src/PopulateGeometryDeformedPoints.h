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
				buf[idx++] = w.w[2];
				buf[idx++] = 1.0f;
			}
	}
	void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
	}

protected:
	void _fillIndex(unsigned int* indices) override
	{
		uint numIndex = indexSize();
		for (uint i = 0; i < numIndex; ++i)
			indices[i] = i + _vertexOffsetId;
	}
};