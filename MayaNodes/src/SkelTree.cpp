#include <SkelTree.h>
#include <SkelTreeData.h>
#include <SkelDeformedMesh.h>
#include <SkelChainOpDirectionalWind.h>
#include <SkelFoliageOpDirectionalWind.h>

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
	ChainOpBaseP opP = new ChainOpDirectionalWind();
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		(*opP)(chainList, deformedMeshData.chainId, data);

		DeformedMesh deformedMesh(deformedMeshData, pTreeData->pointsList, chainList);
		deformedMesh.deform();
	}
	DELETE_POINTER(opP);
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

Uint SkelTree::boneNum() const
{
	return spaceNum() - chainNum() * 2;
}

Uint SkelTree::jointNum() const
{
	return spaceNum() - chainNum();
}

Uint SkelTree::foliageNum() const
{
	return foliageList.size();
}

CRChain SkelTree::getChain(CUint chainId) const
{
	return chainList[chainId];
}

CRMatrix44 SkelTree::getFoliageMatrix(CUint foliageId) const
{
	return foliageList[foliageId].matrix();
}

void SkelTree::updateFoliages(CRFoliageOpData opData)
{
	foliageList.resize(pTreeData->foliageDataList.size());
	Quat q;
	FoliageOpBaseP opP = new FoliageOpDirectionalWind();
	for (Uint i = 0; i < foliageList.size(); ++i) {
		q = (*opP)(pTreeData->foliageDataList[i], opData);
		foliageList[i].update(pTreeData->foliageDataList[i], q, pTreeData);
	}
	DELETE_POINTER(opP);
}

void SkelTree::buildRest(SkelTreeDataP pTreeData)
{
	reset(pTreeData);
	buildChains();
	computWeights();
}

void SkelTree::buildDeform(SkelTreeDataP pTreeData, CRChainOpData opData)
{
	reset(pTreeData);
	buildChains();
	deform(opData);
}

void SkelTree::buildFoliages(SkelTreeDataP pTreeData, CRFoliageOpData opData)
{
	reset(pTreeData);
	updateFoliages(opData);
}

NS_END