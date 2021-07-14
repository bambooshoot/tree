#pragma once

#include <DrawableBufferBase.h>

class DrawableBufferMesh : public DrawableBufferBase
{
public:
	~DrawableBufferMesh() override {}
	bool hasInstance() const override
	{
		return false;
	}

protected:
	bool hasNormal() const override
	{
		return true;
	}
	bool hasColor() const override
	{
		return false;
	}
	uint vertexNum() const override
	{
		return  _param.pTreeData->deformedPointNum();
	}
	uint indexNum() const override
	{
		uint vtxNum = 0;
		for (auto& deformedData : _param.pTreeData->deformedDataList) {
			vtxNum += _param.pPopGeoData->triangleVtx[deformedData.pointsId].length();
		}

		return  vtxNum;
	}
	void fillPositions(float* buf) override
	{
		uint idx = 0;
		for (auto& deformedData : _param.pTreeData->deformedDataList)
			for (auto& p : _param.pTreeData->pointsList[deformedData.pointsId].finalPositions()) {
				buf[idx++] = p.x;
				buf[idx++] = p.y;
				buf[idx++] = p.z;
			}
	}

	void fillColors(float* buf) override
	{
		uint idx = 0;
		for (auto& deformedData : _param.pTreeData->deformedDataList)
			for (auto& w : deformedData.wList) {
				buf[idx++] = w.w.w[0];
				buf[idx++] = w.w.w[1];
				buf[idx++] = w.w.w[2];
				buf[idx++] = 1.0f;
			}
	}
	void fillNormals(float* buf) override
	{
		size_t idx = 0;
		uint vtxNum = vertexNum();
		uint vtxSize = vtxNum * 3;
		for (uint i = 0; i < vtxSize; ++i)
			buf[idx + i] = 0;

		skelTree::Vec n;
		for (auto& deformedData : _param.pTreeData->deformedDataList) {
			MIntArray& vtxArray = _param.pPopGeoData->triangleVtx[deformedData.pointsId];
			skelTree::CRVecList pList = _param.pTreeData->pointsList[deformedData.pointsId].finalPositions();
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
	void fillIndices(unsigned int* indices) override
	{
		uint idx = 0;
		uint currentBeginId = 0;
		for (auto& deformedData : _param.pTreeData->deformedDataList) {
			MIntArray& vtxArray = _param.pPopGeoData->triangleVtx[deformedData.pointsId];
			for (uint i = 0; i < vtxArray.length(); ++i) {
				indices[idx++] = vtxArray[i] + currentBeginId;
			}
			currentBeginId += _param.pTreeData->pointsList[deformedData.pointsId].pointNum();
		}
	}
};