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
	void instanceMatrices(MMatrixArray& matArray, DrawableBufferParam& param) const override
	{
		matArray.clear();

		MMatrix mat;
		skelTree::Matrix44 scaleMatrix;
		skelTree::Vec scale(1, param.pPopGeoData->chainAxisScale, param.pPopGeoData->chainAxisScale);
		for (uint i = 0; i < param.pTree->chainNum(); ++i) {
			auto& chain = param.pTree->getChain(i);
			for (uint j = 0; j < chain.boneNum(); ++j) {
				skelTree::Matrix44 mat44 = chain.jointMatrix(j);
				scale.x = chain.xLen(j);
				scaleMatrix.setScale(scale);
				mat44 = scaleMatrix * mat44;
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
	uint vertexNum(DrawableBufferParam& param) const override
	{
		return  BOX_POINT_6SIDE_NUM;
	}
	uint indexNum(DrawableBufferParam& param) const override
	{
		return  BOX_TRIANGLE_IDX_NUM;
	}
	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		for (uint pId = 0; pId < BOX_POINT_DATA_SIZE; ++pId)
			buf[pId] = skelTree::GeoBox::points[pId];
	}
	void fillColors(float* buf, DrawableBufferParam& param) override
	{
		for (uint j = 0; j < BOX_COLOR_DATA_SIZE; ++j)
			buf[j] = skelTree::GeoBox::colors[j];
	}
	void fillIndices(unsigned int* indices, DrawableBufferParam& param) override
	{
		for (uint j = 0; j < BOX_TRIANGLE_IDX_NUM; ++j)
			indices[j] = skelTree::GeoBox::triIndices[j];
	}
};