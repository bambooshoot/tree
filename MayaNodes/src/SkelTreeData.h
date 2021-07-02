#pragma once

#include <SkelTreeBase.h>
#include <SkelPoints.h>
#include <SkelChainData.h>
#include <SkelDeformedMeshData.h>

NS_BEGIN

STRUCT(SkelTreeData,
	PointsList pointsList;
ChainDataList chainDataList;
DeformedMeshDataList deformedDataList;
Box boundBox;
void reset()
{
	pointsList.clear();
	chainDataList.clear();
	deformedDataList.clear();
};
RPoints addPoints()
{
	pointsList.push_back(Points());
	return pointsList.back();
};
RChainData addChainData()
{
	chainDataList.push_back(ChainData());
	return chainDataList.back();
};
RDeformedMeshData addDeformedData()
{
	deformedDataList.push_back(DeformedMeshData());
	return deformedDataList.back();
};
void computeBox()
{
	boundBox.makeEmpty();
	for (auto& points : pointsList) {
		for (auto& p : points.rest()) {
			boundBox.extendBy(p);
		}
	}
};
Uint pointNum() const
{
	Uint pNum = 0;
	for (auto& points : pointsList)
		pNum += points.pointNum();
	return pNum;
};
Uint deformedPointNum() const
{
	Uint pNum = 0;
	for (auto& deformedData : deformedDataList)
		pNum += pointsList[deformedData.pointsId].pointNum();
	return pNum;
};
)

NS_END