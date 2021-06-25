#pragma once

#include <SkelSpace.h>
#include <SkelChainData.h>

NS_BEGIN

CLASSDERIVED(AttachedPoint, Space,
public:
	AttachedPoint(CAttachedPointDataP pData, CChainDataP pChainData);
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float zParam() const override;

	static SpaceP create(CVoidP data, CChainDataP pChainData);
	static SpaceTypeId typeId;

private:
	CAttachedPointDataP _data;
	CPointsListP        _pPointsList;

	CRVecList _points() const;
	)

NS_END