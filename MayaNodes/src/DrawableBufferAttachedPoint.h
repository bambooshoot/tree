#pragma once

#include <DrawableBufferBase.h>

class DrawableBufferAttachedPoint : public DrawableBufferBase
{
public:
	~DrawableBufferAttachedPoint() override {};
	bool hasInstance() const override
	{
		return true;
	}
	void instanceMatrices(MMatrixArray& matArray, DrawableBufferParam& param) const override
	{
		matArray.clear();

		MMatrix mat;
		for (uint i = 0; i < param.pTree->foliageNum(); ++i) {
			skelTree::CRMatrix44 mat44 = param.pTree->getFoliageMatrix(i);
			skelTree::MatrixTranslate<skelTree::CMatrix44, MMatrix, 4>(mat44, mat);
			matArray.append(mat);
		}
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
	uint indexNum(DrawableBufferParam& param) const override
	{
		return 6;
	}

	uint vertexNum(DrawableBufferParam& param) const override
	{
		return 4;
	}

	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		uint idx = 0;

		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = param.pPopGeoData->chainAxisScale; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = param.pPopGeoData->chainAxisScale; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = param.pPopGeoData->chainAxisScale;
	}

	void fillColors(float *buf, DrawableBufferParam& param) override
	{
		uint idx = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 1; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 1; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 1;
	}

	void fillIndices(uint* indices, DrawableBufferParam& param) override
	{
		uint idx = 0;
		indices[idx++] = 0; indices[idx++] = 1; indices[idx++] = 0;
		indices[idx++] = 2; indices[idx++] = 0; indices[idx++] = 3;
	}
};