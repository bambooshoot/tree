#pragma once

#include <DrawableBufferBase.h>

class DrawableBufferDeformedPoints : public DrawableBufferBase
{
public:
	~DrawableBufferDeformedPoints() override {}
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
		return  param.pTreeData->deformedPointNum();
	}
	uint indexNum(DrawableBufferParam& param) const override
	{
		return  param.pTreeData->deformedPointNum();
	}
	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		uint idx = 0;
		for (auto& deformedData : param.pTreeData->deformedDataList)
			for (auto& p : param.pTreeData->pointsList[deformedData.pointsId].finalPositions()) {
				buf[idx++] = p.x;
				buf[idx++] = p.y;
				buf[idx++] = p.z;
			}
	}

	void fillColors(float* buf, DrawableBufferParam& param) override
	{
		uint idx = 0;
		for (auto& deformedData : param.pTreeData->deformedDataList)
			for (auto& w : deformedData.wList) {
				buf[idx++] = w.w.w[0];
				buf[idx++] = w.w.w[1];
				buf[idx++] = w.w.w[2];
				buf[idx++] = 1.0f;
			}
	}
	void fillIndices(unsigned int* indices, DrawableBufferParam& param) override
	{
		uint numIndex = indexNum(param);
		for (uint i = 0; i < numIndex; ++i)
			indices[i] = i;
	}
};