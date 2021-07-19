#pragma once

#include <SkelTreeBase.h>
#include <SkelChain.h>
#include <SkelPoints.h>
#include <SkelDeformedMeshData.h>

NS_BEGIN

class DeformedMesh
{
public:
	DeformedMesh(RDeformedMeshData deformedMeshData, RPointsList pointsList, CRChainList chainList)
		:	pDeformedMeshData(&deformedMeshData)
	{
		pChain = &chainList[pDeformedMeshData->chainId];
		pPoints = &pointsList[pDeformedMeshData->pointsId];
	}

	void computeWeights();
	void deform();

private:
	DeformedMeshDataP	pDeformedMeshData;
	PointsP				pPoints;
	CChainP				pChain;

	//Uint _nearestSpace(CRVec p) const;
	//void _setWeight(RDeformedMeshWeight w, CRVec p, CUint id);
	//void _locPAndXLen(RVec locP, CRVec p, CUint id) const;
};

NS_END