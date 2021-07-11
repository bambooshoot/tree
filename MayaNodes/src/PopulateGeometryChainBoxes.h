#pragma once

#include <PopulateGeometryBase.h>
#include <SkelGeoBox.h>

class PopulateGeometryChainBoxes : public PopulateGeometryBase
{
public:
	~PopulateGeometryChainBoxes() override {}
	uint vertexSize() const
	{
		return  _pTree->boneNum() * BOX_POINT_6SIDE_NUM;
	}
	uint indexSize() const
	{
		return  _pTree->boneNum() * BOX_TRIANGLE_IDX_NUM;
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = 0;
		skelTree::Vec p;
		uint pId3;
		for (uint i = 0; i < _pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			for (uint boneId = 0; boneId < chain.boneNum(); ++boneId) {
				pId3 = 0;
				skelTree::CRMatrix44 mat = chain.jointMatrix(boneId);
				for (uint pId = 0; pId < BOX_POINT_6SIDE_NUM; ++pId) {
					p.x = skelTree::GeoBox::points[pId3++] * chain.xLen(boneId);
					p.y = skelTree::GeoBox::points[pId3++] * _pPopGeoData->chainAxisScale;
					p.z = skelTree::GeoBox::points[pId3++] * _pPopGeoData->chainAxisScale;
					p *= mat;

					buf[idx++] = p.x;
					buf[idx++] = p.y;
					buf[idx++] = p.z;
				}
			}
		}
	}
	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = 0;
		const uint boneNum = _pTree->boneNum();
		for (uint i = 0; i < boneNum; ++i) {
			for (uint j = 0; j < BOX_COLOR_DATA_SIZE; ++j)
				buf[idx++] = skelTree::GeoBox::colors[j];
		}
	}
	void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
	}
protected:
	void _fillIndex(unsigned int* indices)
	{
		uint idx = 0;
		uint indexBeginId = 0;
		const uint boneNum = _pTree->boneNum();
		for (uint i = 0; i < boneNum; ++i, indexBeginId += BOX_POINT_6SIDE_NUM) {
			for (uint j = 0; j < BOX_TRIANGLE_IDX_NUM; ++j)
				indices[idx++] = skelTree::GeoBox::triIndices[j] + indexBeginId;
		}
	}
};