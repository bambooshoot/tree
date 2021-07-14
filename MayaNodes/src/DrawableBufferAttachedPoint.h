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
	void instanceMatrices(MMatrixArray& matArray) const override
	{
		matArray.clear();

		MMatrix mat;
		for (uint i = 0; i < _param.pTree->foliageNum(); ++i) {
			skelTree::CRMatrix44 mat44 = _param.pTree->getFoliageMatrix(i);
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
	uint indexNum() const override
	{
		return 6;
	}

	uint vertexNum() const override
	{
		return 4;
	}

	void fillPositions(float* buf) override
	{
		uint idx = 0;

		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = _param.pPopGeoData->chainAxisScale; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = _param.pPopGeoData->chainAxisScale; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = _param.pPopGeoData->chainAxisScale;
	}

	void fillColors(float *buf) override
	{
		uint idx = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 1; buf[idx++] = 0; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 1; buf[idx++] = 0;
		buf[idx++] = 0; buf[idx++] = 0; buf[idx++] = 1;
	}

	void fillIndices(uint* indices) override
	{
		uint idx = 0;
		indices[idx++] = 0; indices[idx++] = 1; indices[idx++] = 0;
		indices[idx++] = 2; indices[idx++] = 0; indices[idx++] = 3;
	}
};