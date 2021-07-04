#pragma once

#include <SkelChain.h>
#include <SkelTreeData.h>
#include <SkelChainOpBase.h>

NS_BEGIN

CLASS(SkelTree,
public:
	SkelTree() = default;
	SkelTree(SkelTreeDataP treeData);
	void reset(SkelTreeDataP treeData);
	void buildChains();
	void computWeights();

	void deform(CRChainOpData data);

	CRMatrix44 space(CUint chainId, CUint spaceId) const;
	CRVecList finalPositions(CUint pointsId) const;

	Uint chainNum() const;
	Uint spaceNum() const;
	Uint jointNum() const;
	CRChain getChain(CUint chainId) const;

private:
	SkelTreeDataP	pTreeData;
	ChainList		chainList;
	)

NS_END