#include <SkelAttachedPoint.h>

NS_BEGIN

AttachedPoint::AttachedPoint()
	: _data(nullptr),
	_pPointsList(nullptr) 
{
};

void AttachedPoint::reset(CAttachedPointDataP pData, CSkelTreeDataP pTreeData)
{
	_data = pData;
	_pPointsList = &pTreeData->pointsList;
}

CRVecList AttachedPoint::_points() const
{
	return (*_pPointsList)[_data->pointsId].finalPositions();
}

CRVecList AttachedPoint::_restPoints() const
{
	return (*_pPointsList)[_data->pointsId].crest();
}

Vec AttachedPoint::point(CRVecList pnts) const
{
	Vec p3[3] = { pnts[_data->vid[0]], pnts[_data->vid[1]], pnts[_data->vid[2]] };
	return p3[0] * _data->w[0] + p3[1] * _data->w[1] + p3[2] * (1 - _data->w[0] - _data->w[1]);
}

Vec AttachedPoint::point() const
{
	CRVecList pnts = _points();
	return point(pnts);
}

Vec AttachedPoint::restPoint() const
{
	CRVecList pnts = _restPoints();
	return point(pnts);
}

Matrix44 AttachedPoint::matrix() const
{
	if (_data == nullptr || _pPointsList == nullptr) {
		return Matrix44();
	}
		

	CRVecList pnts = _points();
	Vec p3[3] = { pnts[_data->vid[0]], pnts[_data->vid[1]], pnts[_data->vid[2]] };
	Vec xAxis = p3[1] - p3[0];
	Vec zAxis = p3[2] - p3[0];

	Vec orgP = p3[0] * _data->w[0] + p3[1] * _data->w[1] + p3[2] * (1 - _data->w[0] - _data->w[1]);

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

NS_END