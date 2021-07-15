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
	uint vertexNum(DrawableBufferParam& param) const override
	{
		return  param.pTree->jointNum();
	}
	uint indexNum(DrawableBufferParam& param) const override
	{
		uint chainNum = param.pTree->chainNum();
		uint indexNum = 0;
		for (uint i = 0; i < chainNum; ++i) {
			skelTree::CRChain chain = param.pTree->getChain(i);
			indexNum += (chain.jointNum() - 1) * 2;
		}
		return indexNum;
	}
	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		skelTree::Vec p;
		uint idx = 0;
		for (uint i = 0; i < param.pTree->chainNum(); ++i) {
			skelTree::CRChain chain = param.pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				skelTree::CRMatrix44 mat = chain.jointMatrix(jointId);
				p = mat.translation();
				buf[idx++] = p.x; buf[idx++] = p.y; buf[idx++] = p.z;
			}
		}
	}
	void fillColors(float* buf, DrawableBufferParam& param) override
	{
		uint idx = 0;
		float u;
		for (uint i = 0; i < param.pTree->chainNum(); ++i) {
			skelTree::CRChain chain = param.pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum(); ++jointId) {
				u = chain.xParam(jointId);
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = u;
				buf[idx++] = 1.0f;
			}
		}
	}
	void fillIndices(unsigned int* indices, DrawableBufferParam& param) override
	{
		uint chainNum = param.pTree->chainNum();
		uint indexBeginId = 0;
		uint idx = 0;
		for (uint i = 0; i < chainNum; ++i) {
			skelTree::CRChain chain = param.pTree->getChain(i);
			for (uint jointId = 0; jointId < chain.jointNum() - 1; ++jointId) {
				indices[idx++] = jointId + indexBeginId;
				indices[idx++] = jointId + 1 + indexBeginId;
			}
			indexBeginId += chain.jointNum();
		}
	}
};