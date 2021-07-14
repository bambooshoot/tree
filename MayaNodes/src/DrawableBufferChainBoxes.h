#pragma once

#include <DrawableBufferBase.h>
#include <SkelGeoBox.h>

class DrawableBufferChainBoxes : public DrawableBufferBase
{
public:
	~DrawableBufferChainBoxes() override {}
	bool hasInstance() const override
	{
		return true;
	}
	void instanceMatrices(MMatrixArray& matArray) const override
	{
		matArray.clear();

		MMatrix mat;
		for (uint i = 0; i < _param.pTree->chainNum(); ++i) {
			auto& chain = _param.pTree->getChain(i);
			for (uint j = 0; j < chain.jointNum(); ++j) {
				skelTree::CRMatrix44 mat44 = chain.jointMatrix(j);
				skelTree::MatrixTranslate<skelTree::CMatrix44, MMatrix, 4>(mat44, mat);
				matArray.append(mat);
			}
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
	uint vertexNum() const override
	{
		return  BOX_POINT_6SIDE_NUM;
	}
	uint indexNum() const override
	{
		return  BOX_TRIANGLE_IDX_NUM;
	}
	void fillPositions(float* buf) override
	{
		for (uint pId = 0; pId < BOX_POINT_DATA_SIZE; ++pId)
			buf[pId] = skelTree::GeoBox::points[pId];
	}
	void fillColors(float* buf) override
	{
		for (uint j = 0; j < BOX_COLOR_DATA_SIZE; ++j)
			buf[j] = skelTree::GeoBox::colors[j];
	}
	void fillIndices(unsigned int* indices) override
	{
		for (uint j = 0; j < BOX_TRIANGLE_IDX_NUM; ++j)
			indices[j] = skelTree::GeoBox::triIndices[j];
	}
};