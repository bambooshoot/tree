#include <SkelAttachedPoint.h>

NS_BEGIN

SpaceTypeId AttachedPoint::typeId(0x01);

SpaceP AttachedPoint::create(CVoidP data, CSkelTreeDataP pTreeData)
{
	return new AttachedPoint((CAttachedPointDataP)data, pTreeData);
}

AttachedPoint::AttachedPoint(CAttachedPointDataP pData, CSkelTreeDataP pTreeData)
{
	_data = pData;
	_pPointsList = &pTreeData->pointsList;
}

CRVecList AttachedPoint::_points() const
{
	return (*_pPointsList)[_data->pointsId].finalPositions();
}

Matrix44 AttachedPoint::restMatrix() const
{
	CRVecList pnts = _points();
	Vec p3[3] = { pnts[_data->vid[0]], pnts[_data->vid[1]], pnts[_data->vid[2]] };
	Vec xAxis = p3[1] - p3[0];
	Vec zAxis = p3[2] - p3[0];

	Vec orgP = p3[0] + xAxis * _data->w[0] + zAxis * _data->w[1];

	Vec yAxis = xAxis.cross(zAxis);
	yAxis.normalize();
	xAxis.normalize();
	zAxis = xAxis.cross(yAxis);
	zAxis.normalize();

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