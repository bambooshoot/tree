#include <SkelTree.h>
#include <SkelTreeData.h>
#include <SkelDeformedMesh.h>
#include <SkelChainOpDirectionalWind.h>

NS_BEGIN

SkelTree::SkelTree(SkelTreeDataP treeData)
{
	reset(treeData);
}

void SkelTree::reset(SkelTreeDataP treeData)
{
	pTreeData = treeData;
}

void SkelTree::buildChains()
{
	chainList.clear();
	for (auto& chainData : pTreeData->chainDataList) {
		chainList.push_back(Chain());
	}

	auto iter = pTreeData->chainDataList.begin();
	for (auto& chain : chainList) {
		chain.build(*iter, pTreeData);
		++iter;
	}
}

CRMatrix44 SkelTree::space(CUint chainId, CUint spaceId) const
{
	return chainList[chainId].matrix(spaceId);
}

CRVecList SkelTree::finalPositions(CUint pointsId) const
{
	return pTreeData->pointsList[pointsId].finalPositions();
}

void SkelTree::computWeights()
{
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		DeformedMesh deformedMesh(deformedMeshData, pTreeData->pointsList, chainList);
		deformedMesh.computeWeights();
	}
}

void SkelTree::deform(CRChainOpData data)
{
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		ChainOpBaseP opP = new ChainOpDirectionalWind();
		(*opP)(chainList, deformedMeshData.chainId, data);
		DELETE_POINTER(opP);

		DeformedMesh deformedMesh(deformedMeshData, pTreeData->pointsList, chainList);
		deformedMesh.deform();
	}
}

Uint SkelTree::chainNum() const
{
	return Uint(chainList.size());
}

Uint SkelTree::spaceNum() const
{
	Uint spNum = 0;
	for (auto& chain : chainList) {
		spNum += chain.spaceNum();
	}
	return spNum;
}

Uint SkelTree::jointNum() const
{
	return spaceNum() - chainNum();
}

CRChain SkelTree::getChain(CUint chainId) const
{
	return chainList[chainId];
}

NS_END