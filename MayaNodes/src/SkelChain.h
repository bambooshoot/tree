#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

NS_BEGIN

CLASS(Chain,
public:
	~Chain();
	void build( CRChainData chainData, CSkelTreeDataP pTreeData, CMatrix44P pMat = nullptr);

	void updateMatrix(CRQuatList qList, CMatrix44P pRootMat = nullptr);

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
	CRMatrix44 jointDeformMatrix(CUint spaceId) const;

	Uint boneNum() const;
	CFloat xLen(CUint boneId) const;

private:
	SpacePList		jointList;

	// cached data
	Matrix44List	restMatrixList;
	Matrix44List	restInvMatrixList;
	Matrix44List	matrixList;
	Matrix44List	deformMatrixList; //the result of restInvMatrix * matrix that is multiplied frequently
									  //store them can avoid huge repetitive computing time;
	FloatList		xParamList;

	void _updateParam();
	void _updateRestMatrix(CMatrix44P pRootMat = nullptr);
	)

NS_END