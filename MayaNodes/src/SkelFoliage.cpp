#include <SkelFoliage.h>
#include <SkelAttachedPoint.h>
#include <SkelChainOpDirectionalWind.h>

NS_BEGIN

void Foliage::update(CUint foliageId, CSkelTreeDataP pTreeData, CRAniOpData opData, CAniOpBaseP pAniOp)
{
	CRFoliageData foliageData = pTreeData->foliageDataList[foliageId];

	AttachedPoint ap;
	ap.reset(&foliageData.attachPoint, pTreeData);
	Vec p = ap.point();
	Quat q1 = foliageData.q;

	AniOpState state;
	state.uIdx = foliageId;
	state.fIdx = Float(foliageId);
	state.p = ap.restPoint();

	Quat q = (*pAniOp).computeQ(state, opData);

	q1 = q * q1;

	_matrix = Matrix44();
	_matrix.setScale(foliageData.scale);
	_matrix *= q1.toMatrix44();
	_matrix[3][0] = p.x;
	_matrix[3][1] = p.y;
	_matrix[3][2] = p.z;
}

CRMatrix44 Foliage::matrix() const
{
	return _matrix;
}

NS_END