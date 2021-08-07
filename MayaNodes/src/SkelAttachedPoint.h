#pragma once

#include <SkelTreeData.h>

NS_BEGIN

CLASS(AttachedPoint,
public:
	AttachedPoint();
	void reset(CAttachedPointDataP pData, CSkelTreeDataP pTreeData);
	Matrix44 matrix() const;
	Vec point() const;
	Vec restPoint() const;

private:
	CAttachedPointDataP _data;
	CPointsListP        _pPointsList;

	CRVecList _points() const;
	CRVecList _restPoints() const;
	Vec point(CRVecList pnts) const;
	)

NS_END