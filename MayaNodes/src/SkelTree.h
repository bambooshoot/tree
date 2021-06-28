#pragma once

#include <SkelTreeBase.h>
#include <SkelChain.h>
#include <SkelPoints.h>
#include <SkelChainData.h>
#include <SkelDeformedMesh.h>

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
	}
	RChainData addChainData()
	{
		chainDataList.push_back(ChainData());
		return chainDataList.back();
	}
	RDeformedMeshData addDeformedData()
	{
		deformedDataList.push_back(DeformedMeshData());
		return deformedDataList.back();
	}
	void computeBox()
	{
		boundBox.makeEmpty();
		for (auto& points : pointsList) {
			for (auto& p : points.rest()) {
				boundBox.extendBy(p);
			}
		}
	}
	Uint pointNum() const
	{
		Uint pNum = 0;
		for (auto& points : pointsList)
			pNum += points.pointNum();
		return pNum;
	}
	)

class SkelTree {
public:
	// initialize
	SkelTree(RSkelTreeData treeData);
	void buildChains();
	void computWeights();

	// frame update
	void updateChains(CFloat time);
	void deform();

	// output
	CRMatrix44 space(CUint chainId, CUint spaceId) const;
	CRVecList finalPositions(CUint pointsId) const;

private:
	SkelTreeDataP	pTreeData;
	ChainList		chainList;
};

NS_END