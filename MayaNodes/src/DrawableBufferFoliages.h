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
		return true;
	}
	bool hasColor() const override
	{
		return false;
	}
	uint vertexNum() const override
	{
		uint pointsId = _param.pTreeData->foliageDataList[0].pointsId;
		return _param.pTreeData->pointsList[pointsId].pointNum();
	}
	uint indexNum() const override
	{
		uint pointsId = _param.pTreeData->foliageDataList[0].pointsId;
		return _param.pPopGeoData->triangleVtx[pointsId].length();
	}
	void fillPositions(float* buf) override
	{
		uint pointsId = _param.pTreeData->foliageDataList[0].pointsId;
		uint idx = 0;
		for (auto& p : _param.pTreeData->pointsList[pointsId].rest()) {
			buf[idx++] = p.x;
			buf[idx++] = p.y;
			buf[idx++] = p.z;
		}
	}
	void fillNormals(float* buf) override
	{
		static const skelTree::Vec n0(0, 0, 1.0f);
		uint pntNum = vertexNum();
		uint idx = 0;
		for (uint i = 0; i < pntNum; ++i) {
			buf[idx++] = n0.x;
			buf[idx++] = n0.y;
			buf[idx++] = n0.z;
		}
	}

	void fillIndices(unsigned int* indices) override
	{
		uint pointsId = _param.pTreeData->foliageDataList[0].pointsId;
		MIntArray& vtxArray = _param.pPopGeoData->triangleVtx[pointsId];
		for (uint i = 0; i < vtxArray.length(); ++i) {
			indices[i] = vtxArray[i];
		}
	}
};