#include <SkelAttachedPoint.h>

NS_BEGIN

SpaceTypeId AttachedPoint::typeId(0x01);

SpaceP AttachedPoint::create(CVoidP data, CChainDataP pChainData)
{
	return new AttachedPoint((CAttachedPointDataP)data, pChainData);
}

AttachedPoint::AttachedPoint(CAttachedPointDataP pData, CChainDataP pChainData)
{
	_data = pData;
}

CRVecList AttachedPoint::_points() const
{
	return (*_pPointsList)[_data->pointsId].finalPositions();
}

Matrix44 AttachedPoint::restMatrix() const
{
	CRVecList pnts = _points();
	Vec xAxis = pnts[1] - pnts[0];
	Vec zAxis = pnts[2] - pnts[0];
	Vec yAxis = xAxis.cross(zAxis);
	yAxis.normalize();
	xAxis.normalize();
	zAxis = xAxis.cross(yAxis);
	zAxis.normalize();

	Vec orgP(0, 0, 0);
	auto pIter = pnts.begin();
	for (auto wIter = _data->wList.begin(); wIter != _data->wList.end(); ++wIter, ++pIter) {
		orgP += *pIter * wIter->w;
	}

	return Matrix44(
		xAxis.x, xAxis.y, xAxis.z, 0.0f,
		yAxis.x, yAxis.y, yAxis.z, 0.0f,
		zAxis.x, zAxis.y, zAxis.z, 0.0f,
		orgP.x, orgP.y, orgP.z, 1 );
}

Matrix44 AttachedPoint::matrix(CRQuat q) const
{
	return restMatrix();
}

Float AttachedPoint::xParam() const
{
	return 0.0f;
}

NS_END