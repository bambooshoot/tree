#include <SkelFoliage.h>
#include <SkelAttachedPoint.h>

NS_BEGIN

void Foliage::update(CRFoliageData foliageData, CRQuat q, CSkelTreeDataP pTreeData)
{
	AttachedPoint ap;
	ap.reset(&foliageData.attachPoint, pTreeData);
	Quat q1 = q * foliageData.q;
	_matrix = ap.matrix() * q1.toMatrix44();
}

CRMatrix44 Foliage::matrix() const
{
	return _matrix;
}

NS_END