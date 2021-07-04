#pragma once
#include <PopulateGeometryBase.h>

class PopulateGeometryChainLine : public PopulateGeometryBase
{
public:
	uint vertexSize() const
	{
		return  _pTree->jointNum();
	}
	uint indexSize() const
	{
		uint chainNum = _pTree->chainNum();
		uint indexNum = 0;
		for (uint i = 0; i < chainNum; ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			indexNum += (chain.jointNum() - 1) * 2;
		}
		return indexNum;
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		skelTree::Vec p;
		uint idx = _positionOffsetId();
		for (uint i = 0; i < _pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				skelTree::CRMatrix44 mat = chain.jointMatrix(jointId);
				p = mat.translation();
				buf[idx++] = p.x; buf[idx++] = p.y; buf[idx++] = p.z;
			}
		}
	}
	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = _colorOffsetId();
		float u;
		for (uint i = 0; i < _pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				u = chain.xParam(jointId);
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = 1.0f;
			}
		}
	}
	void populateGeometryNormal(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
	}

protected:
	void _fillIndex(unsigned int* indices)
	{
		uint chainNum = _pTree->chainNum();
		uint indexBeginId = _vertexOffsetId;
		uint idx = 0;
		for (uint i = 0; i < chainNum; ++i) {
			skelTree::CRChain chain = _pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum() - 1; ++jointId) {
				indices[idx++] = jointId + indexBeginId;
				indices[idx++] = jointId + 1 + indexBeginId;
			}
			indexBeginId += chain.jointNum();
		}
	}
};