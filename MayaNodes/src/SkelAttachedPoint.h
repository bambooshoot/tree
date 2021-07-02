#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

NS_BEGIN

CLASSDERIVED(AttachedPoint, Space,
public:
	~AttachedPoint() override {};
	AttachedPoint(CAttachedPointDataP pData, CSkelTreeDataP pTreeData);
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float xParam() const override;

	static SpaceP create(CVoidP data, CSkelTreeDataP pTreeData);
	static SpaceTypeId typeId;

private:
	CAttachedPointDataP _data;
	CPointsListP        _pPointsList;

	CRVecList _points() const;
	)

NS_END