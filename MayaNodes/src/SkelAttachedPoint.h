#pragma once

#include <SkelTreeData.h>

NS_BEGIN

CLASS(AttachedPoint,
public:
	AttachedPoint();
	void reset(CAttachedPointDataP pData, CSkelTreeDataP pTreeData);
	Matrix44 matrix() const;
	Vec point() const;

private:
	CAttachedPointDataP _data;
	CPointsListP        _pPointsList;

	CRVecList _points() const;
	)

NS_END