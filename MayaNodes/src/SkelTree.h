#pragma once

#include <SkelChain.h>
#include <SkelTreeData.h>
#include <SkelChainOpBase.h>
#include <SkelFoliage.h>
#include <SkelChainOpBase.h>
#include <SkelChainOpDirectionalWind.h>

NS_BEGIN

CLASS(SkelTree,
public:
	SkelTree() = default;
	~SkelTree();
	SkelTree(SkelTreeDataP treeData);
	
	void buildRest(SkelTreeDataP treeData);
	void buildStruct(SkelTreeDataP treeData);
	void deformAndFoliages(SkelTreeDataP treeData, CRAniOpData data, bool bUpdateFoliage);

	void reset(SkelTreeDataP treeData);

	CRMatrix44 space(CUint chainId, CUint spaceId) const;
	CRVecList finalPositions(CUint pointsId) const;

	Uint chainNum() const;
	Uint spaceNum() const;
	Uint jointNum() const;
	Uint boneNum() const;
	Uint foliageNum() const;
	CRChain getChain(CUint chainId) const;
	CRMatrix44 getFoliageMatrix(CUint foliageId) const;

private:
	SkelTreeDataP	pTreeData;
	ChainList		chainList;
	FoliageList     foliageList;

	static UAniOpBasePtr      trunkOp;
	static UAniOpBasePtr      branchOp;
	static UAniOpBasePtr      foliageOp;

	void buildChains();
	void computWeights();

	void deform(CRAniOpData data);
	void updateFoliages(CRAniOpData data);
	void clear();
	)

NS_END