#pragma once

#include <SkelTreeData.h>
#include <SkelChainOpBase.h>

NS_BEGIN

CLASS(Foliage,
public:
	void update(CUint foliageId, CSkelTreeDataP pTreeData, CRAniOpData opData, CAniOpBaseP pAniOp);
	CRMatrix44 matrix() const;

private:
	Matrix44 _matrix;
	)

NS_END