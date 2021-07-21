#include <SkelTree.h>
#include <SkelTreeData.h>
#include <SkelDeformedMesh.h>

NS_BEGIN

UAniOpBasePtr SkelTree::trunkOp = MAKE_UNIQUE<TrunkOpDirectionalWind>();
UAniOpBasePtr SkelTree::branchOp = MAKE_UNIQUE<BranchOpDirectionalWind>();
UAniOpBasePtr SkelTree::foliageOp = MAKE_UNIQUE<FoliageOpDirectionalWind>();

SkelTree::~SkelTree()
{
	clear();
}
void SkelTree::clear()
{
}
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

	// build trunk chain at first.
	CUint parentChainId = pTreeData->trunkChainId;
	chainList[parentChainId].build(pTreeData->chainDataList[parentChainId], pTreeData);

	// then build branch chains with matrix of parent frame of trunk chain.
	auto iter = chainList.begin();
	CRChain parentChain = chainList[parentChainId];
	for (Uint i=0;i<chainList.size();++i) {
		if (i == parentChainId)
			continue;

		CRChainData chainData = pTreeData->chainDataList[i];
		CRMatrix44 parentMat = parentChain.jointRestMatrix(chainData.trunkFrameId);
		(iter+i)->build(chainData, pTreeData, &parentMat);
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

void SkelTree::deform(CRAniOpData data)
{
	for (auto& deformedMeshData : pTreeData->deformedDataList) {
		RChain chain = chainList[deformedMeshData.chainId];
		
		CUint trunkFrameId = pTreeData->chainDataList[deformedMeshData.chainId].trunkFrameId;
		if (trunkFrameId != USHORT_MAX) {
			RChain rootChain = chainList[pTreeData->trunkChainId];
			CRMatrix44 mat = rootChain.jointMatrix(trunkFrameId);

			QuatList qList = SkelTree::branchOp->chainOp(chain, deformedMeshData.chainId, data);
			chain.updateMatrix(qList, &mat);
		}
		else {
			QuatList qList = SkelTree::trunkOp->chainOp(chain, deformedMeshData.chainId, data);
			chain.updateMatrix(qList);
		}

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
	return Uint(foliageList.size());
}

CRChain SkelTree::getChain(CUint chainId) const
{
	return chainList[chainId];
}

CRMatrix44 SkelTree::getFoliageMatrix(CUint foliageId) const
{
	return foliageList[foliageId].matrix();
}

void SkelTree::updateFoliages(CRAniOpData opData)
{
	foliageList.resize(pTreeData->foliageDataList.size());
	
	for (Uint i = 0; i < foliageList.size(); ++i) {
		foliageList[i].update(i, pTreeData, opData, SkelTree::foliageOp.get());
	}
}

void SkelTree::buildRest(SkelTreeDataP pTreeData)
{
	reset(pTreeData);
	buildChains();
	computWeights();
}

void SkelTree::buildStruct(SkelTreeDataP pTreeData)
{
	reset(pTreeData);
	buildChains();
}

void SkelTree::deformAndFoliages(SkelTreeDataP pTreeData, CRAniOpData opData)
{
	reset(pTreeData);
	deform(opData);
	updateFoliages(opData);
}

NS_END