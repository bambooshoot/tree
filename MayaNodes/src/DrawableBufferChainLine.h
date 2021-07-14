#pragma once
#include <DrawableBufferBase.h>

class DrawableBufferChainLine : public DrawableBufferBase
{
public:
	~DrawableBufferChainLine() override {}
	bool hasInstance() const override
	{
		return false;
	}

protected:
	bool hasNormal() const override
	{
		return false;
	}
	bool hasColor() const override
	{
		return true;
	}
	uint vertexNum() const override
	{
		return  _param.pTree->jointNum();
	}
	uint indexNum() const override
	{
		uint chainNum = _param.pTree->chainNum();
		uint indexNum = 0;
		for (uint i = 0; i < chainNum; ++i) {
			skelTree::CRChain chain = _param.pTree->getChain(i);
			indexNum += (chain.jointNum() - 1) * 2;
		}
		return indexNum;
	}
	void fillPositions(float* buf) override
	{
		skelTree::Vec p;
		uint idx = 0;
		for (uint i = 0; i < _param.pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _param.pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				skelTree::CRMatrix44 mat = chain.jointMatrix(jointId);
				p = mat.translation();
				buf[idx++] = p.x; buf[idx++] = p.y; buf[idx++] = p.z;
			}
		}
	}
	void fillColors(float* buf) override
	{
		uint idx = 0;
		float u;
		for (uint i = 0; i < _param.pTree->chainNum(); ++i) {
			skelTree::CRChain chain = _param.pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				u = chain.xParam(jointId);
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = 1.0f;
			}
		}
	}
	void fillIndices(unsigned int* indices) override
	{
		for (uint i = 0; i < vertexNum(); ++i)
			indices[i] = i;
	}
	//void fillIndices(unsigned int* indices) override
	//{
	//	uint chainNum = _param.pTree->chainNum();
	//	uint indexBeginId = 0;
	//	uint idx = 0;
	//	for (uint i = 0; i < chainNum; ++i) {
	//		skelTree::CRChain chain = _param.pTree->getChain(i);
	//		for (uint jointId = 0; jointId < chain.jointNum() - 1; ++jointId) {
	//			indices[idx++] = jointId + indexBeginId;
	//			indices[idx++] = jointId + 1 + indexBeginId;
	//		}
	//		indexBeginId += chain.jointNum();
	//	}
	//}
};