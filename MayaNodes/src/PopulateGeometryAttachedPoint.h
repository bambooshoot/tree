#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryAttachedPoint : public PopulateGeometryBase
{
public:
	uint vertexSize() const
	{
		return  _axisNum() * 4;
	}
	uint indexSize() const
	{
		return  _axisNum() * 6;
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = 0;
		const skelTree::Vec p4[4] = {
			skelTree::Vec(0, 0, 0),
			skelTree::Vec(_pPopGeoData->chainAxisScale, 0, 0),
			skelTree::Vec(0, _pPopGeoData->chainAxisScale, 0),
			skelTree::Vec(0, 0, _pPopGeoData->chainAxisScale)
		};
		skelTree::Vec p1;

		for (uint i = 0; i < _pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			skelTree::Matrix44 mat = chain.matrix(0);
			for (auto& p : p4) {
				p1 = p * mat;
				mat.multVecMatrix(p, p1);
				buf[idx++] = p1.x; buf[idx++] = p1.y; buf[idx++] = p1.z;
			}
		}

		for (uint i = 0; i < _pTree->foliageNum(); ++i) {
			skelTree::Matrix44 mat = _pTree->getFoliageMatrix(i);
			for (auto& p : p4) {
				p1 = p * mat;
				mat.multVecMatrix(p, p1);
				buf[idx++] = p1.x; buf[idx++] = p1.y; buf[idx++] = p1.z;
			}
		}
	}
	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		static const skelTree::Vec c4[4] = {
			skelTree::Vec(0, 0, 0),
			skelTree::Vec(1, 0, 0),
			skelTree::Vec(0, 1, 0),
			skelTree::Vec(0, 0, 1)
		};

		uint idx = 0;
		for (uint i = 0; i < _axisNum(); ++i) {
			for (uint i4 = 0; i4 < 4; ++i4) {
				buf[idx++] = c4[i4].x;
				buf[idx++] = c4[i4].y;
				buf[idx++] = c4[i4].z;
				buf[idx++] = 1.0f;
			}
		}
	}
	void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
	}

protected:
	uint _axisNum() const
	{
		return _pTree->chainNum() + _pTree->foliageNum();
	}
	void _fillIndex(unsigned int* indices) override
	{
		uint idx = 0, frameBeginId = 0;
		static const uint idOffset[6] = { 0,1,0,2,0,3 };
		for (uint i = 0; i < _axisNum(); ++i) {
			for (auto pId : idOffset)
				indices[idx++] = frameBeginId + pId;

			frameBeginId += 4;
		}
	}
};