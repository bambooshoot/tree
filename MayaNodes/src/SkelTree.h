#pragma once

#include <SkelTreeBase.h>
#include <SkelChain.h>
#include <SkelPoints.h>
#include <SkelChainData.h>
#include <SkelDeformedMesh.h>

NS_BEGIN

STRUCT(SkelTreeData,
	PointsList points;
	ChainDataList chainDataList;
	DeformedMeshDataList deformedDataList;
	void reset() 
	{
		points.clear();
		chainDataList.clear();
		deformedDataList.clear();
	};
	RPoints addPoints()
	{
		points.push_back(Points());
		return points.back();
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