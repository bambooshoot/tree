#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryFoliages : public PopulateGeometryBase
{
public:
	uint vertexSize() const
	{
		uint vtxSize = 0;
		for (auto & foliageData : _pTreeData->foliageDataList) {
			vtxSize += _pTreeData->pointsList[foliageData.pointsId].pointNum();
		}
		return vtxSize;
	}
	uint indexSize() const
	{
		uint idxSize = 0;
		for (auto& foliageData : _pTreeData->foliageDataList) {
			idxSize += _pPopGeoData->triangleVtx[foliageData.pointsId].length();
		}
		return idxSize;
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = 0;
		for (auto& foliageData : _pTreeData->foliageDataList)
			for (auto& p : _pTreeData->pointsList[foliageData.pointsId].finalPositions()) {
				buf[idx++] = p.x;
				buf[idx++] = p.y;
				buf[idx++] = p.z;
			}
	}
	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		
	}
	void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		size_t idx = 0;
		uint vtxNum = vertexSize();
		uint vtxSize = vtxNum * 3;
		for (uint i = 0; i < vtxSize; ++i)
			buf[idx + i] = 0;

		skelTree::Vec n;
		for (auto& foliageData : _pTreeData->foliageDataList) {
			MIntArray& vtxArray = _pPopGeoData->triangleVtx[foliageData.pointsId];
			skelTree::CRVecList pList = _pTreeData->pointsList[foliageData.pointsId].finalPositions();
			uint triVtxNum = vtxArray.length();
			for (uint i3 = 0; i3 < triVtxNum; i3 += 3) {
				const uint id0 = vtxArray[i3];
				const uint id1 = vtxArray[i3 + 1];
				const uint id2 = vtxArray[i3 + 2];

				skelTree::CRVec p0 = pList[id0];
				skelTree::CRVec p1 = pList[id1];
				skelTree::CRVec p2 = pList[id2];

				n = (p1 - p0).cross(p2 - p0);

				const uint id03 = id0 * 3;
				const uint id13 = id1 * 3;
				const uint id23 = id2 * 3;

				buf[idx + id03] += n.x; buf[idx + id03 + 1] += n.y; buf[idx + id03 + 2] += n.z;
				buf[idx + id13] += n.x; buf[idx + id13 + 1] += n.y; buf[idx + id13 + 2] += n.z;
				buf[idx + id23] += n.x; buf[idx + id23 + 1] += n.y; buf[idx + id23 + 2] += n.z;
			}
			idx += pList.size() * 3;
		}

		idx = 0;
		for (uint i = 0; i < vtxNum; ++i, idx += 3) {
			n.setValue(buf[idx], buf[idx + 1], buf[idx + 2]);
			n.normalize();
			buf[idx] = n.x;
			buf[idx + 1] = n.y;
			buf[idx + 2] = n.z;
		}

	}

protected:
	void _fillIndex(unsigned int* indices) override
	{
		uint idx = 0;
		uint currentBeginId = 0;
		for (auto& foliageData : _pTreeData->foliageDataList) {
			MIntArray& vtxArray = _pPopGeoData->triangleVtx[foliageData.pointsId];
			for (uint i = 0; i < vtxArray.length(); ++i) {
				indices[idx++] = vtxArray[i] + currentBeginId;
			}
			currentBeginId += _pTreeData->pointsList[foliageData.pointsId].pointNum();
		}
	}
};