#include <SkelTree.h>
#include <SkelChainOpNoise.h>

NS_BEGIN

SkelTree::SkelTree(RSkelTreeData treeData)
{
	pTreeData = &treeData;
	buildChains();
}

void SkelTree::buildChains()
{
	for (auto& chainData : pTreeData->chainDataList) {
		chainList.push_back(Chain());
		RChain chain = chainList.back();
		chain.build(chainData);
	}
}

CRMatrix44 SkelTree::space(CUint chainId, CUint spaceId) const
{
	return chainList[chainId].matrix(spaceId);
}

CRVecList SkelTree::finalPositions(CUint pointsId) const
{
	return pTreeData->points[pointsId].finalPositions();
}

void SkelTree::computWeights()
{
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		DeformedMesh deformedMesh(deformedMeshData, pTreeData->points, chainList);
		deformedMesh.computeWeights();
	}
}

void SkelTree::deform()
{
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		DeformedMesh deformedMesh(deformedMeshData, pTreeData->points, chainList);
		deformedMesh.deform();
	}
}

void SkelTree::updateChains(CFloat time)
{
	ChainOpBaseP opP = new ChainOpNoise();
	Uint chainId = 0;
	for (auto& chain : chainList) {
		(*opP)(chain, chainId, time);
		++chainId;
	}
	delete opP;
}

NS_END