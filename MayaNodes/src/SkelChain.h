#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>
#include <SkelAttachedPoint.h>

NS_BEGIN

CLASS(Chain,
public:
	~Chain();
	void build( CRChainData chainData, CSkelTreeDataP pTreeData);

	void updateMatrix(CRQuatList qList);

	CRMatrix44 matrix(CUint spaceId) const;
	CRMatrix44 restMatrix(CUint spaceId) const;
	CRMatrix44 restInvMatrix(CUint spaceId) const;

	Float xParam(CUint spaceId) const;
	Uint spaceNum() const;

	Uint jointNum() const;
	VecList restJointList() const;
	CRMatrix44 jointMatrix(CUint spaceId) const;
	CRMatrix44 jointRestMatrix(CUint spaceId) const;
	CRMatrix44 jointRestInvMatrix(CUint spaceId) const;

	Uint boneNum() const;
	CFloat xLen(CUint boneId) const;

private:
	AttachedPoint   attachedPoint;
	SpacePList		jointList;

	// cached data
	Matrix44List	restMatrixList;
	Matrix44List	restInvMatrixList;
	Matrix44List	matrixList;
	FloatList		xParamList;

	void _updateParam();
	void _updateRestMatrix();
	)

NS_END