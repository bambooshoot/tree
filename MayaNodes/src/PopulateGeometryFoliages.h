#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryFoliages : public PopulateGeometryBase
{
public:
	~PopulateGeometryFoliages() override {}
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
		uint fid = 0, idx = 0;
		skelTree::Vec wp;
		for (auto& foliageData : _pTreeData->foliageDataList) {
			skelTree::CRMatrix44 mat = _pTree->getFoliageMatrix(fid++);
			for (auto& p : _pTreeData->pointsList[foliageData.pointsId].rest()) {
				wp = p * mat;
				buf[idx++] = wp.x;
				buf[idx++] = wp.y;
				buf[idx++] = wp.z;
			}
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
		static const skelTree::Vec n0(0, 0, 1.0f);
		uint fid = 0;
		for (auto& foliageData : _pTreeData->foliageDataList) {
			skelTree::Matrix44 mat = _pTree->getFoliageMatrix(fid++);
			mat.multDirMatrix(n0, n);

			uint pntNum = _pTreeData->pointsList[foliageData.pointsId].pointNum();
			for (uint i = 0; i < pntNum;++i) {
				buf[idx++] = n.x; 
				buf[idx++] = n.y; 
				buf[idx++] = n.z;
			}
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