#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

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
	Float xLen(CUint spaceId) const;
	Uint spaceNum() const;
	CSpaceP space(CUint spaceId) const;
	VecList restPointList() const;
	VecList pointList() const;

private:
	SpacePList		spaceList;
	Matrix44List	restMatrixList;
	Matrix44List	restInvMatrixList;
	Matrix44List	matrixList;
	FloatList		xParamList;
	FloatList		xLenList;

	void _updateParam();
	void _updateRestMatrix();
	)

NS_END