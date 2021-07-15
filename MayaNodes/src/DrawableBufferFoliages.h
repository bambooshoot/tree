#pragma once

#include <DrawableBufferBase.h>

class DrawableBufferFoliages : public DrawableBufferBase
{
public:
	~DrawableBufferFoliages() override {}
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
		return true;
	}
	bool hasColor() const override
	{
		return false;
	}
	uint vertexNum(DrawableBufferParam& param) const override
	{
		uint pointsId = param.pTreeData->foliageDataList[0].pointsId;
		return param.pTreeData->pointsList[pointsId].pointNum();
	}
	uint indexNum(DrawableBufferParam& param) const override
	{
		uint pointsId = param.pTreeData->foliageDataList[0].pointsId;
		return (*param.pTriangleVtx)[pointsId].length();
	}
	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		uint pointsId = param.pTreeData->foliageDataList[0].pointsId;
		uint idx = 0;
		for (auto& p : param.pTreeData->pointsList[pointsId].rest()) {
			buf[idx++] = p.x;
			buf[idx++] = p.y;
			buf[idx++] = p.z;
		}
	}
	void fillNormals(float* buf, DrawableBufferParam& param) override
	{
		static const skelTree::Vec n0(0, 0, 1.0f);
		uint pntNum = vertexNum(param);
		uint idx = 0;
		for (uint i = 0; i < pntNum; ++i) {
			buf[idx++] = n0.x;
			buf[idx++] = n0.y;
			buf[idx++] = n0.z;
		}
	}

	void fillIndices(unsigned int* indices, DrawableBufferParam& param) override
	{
		uint pointsId = param.pTreeData->foliageDataList[0].pointsId;
		MIntArray& vtxArray = (*param.pTriangleVtx)[pointsId];
		for (uint i = 0; i < vtxArray.length(); ++i) {
			indices[i] = vtxArray[i];
		}
	}
};