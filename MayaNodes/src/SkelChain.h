#pragma once

#include <SkelSpace.h>
#include <SkelChainData.h>

NS_BEGIN

CLASS(Chain,
public:
	~Chain();
	void build( CRChainData chainData );

	void updateMatrix(CRQuatList qList);

	CRMatrix44 matrix(CUint spaceId) const;
	CRMatrix44 restMatrix(CUint spaceId) const;
	Float zParam(CUint spaceId) const;
	Float zLen(CUint spaceId) const;
	Uint spaceNum() const;

private:
	SpacePList		spaceList;
	Matrix44List	restMatrixList;
	Matrix44List	matrixList;
	FloatList		zParamList;

	void _updateParam();
	void _updateRestMatrix();
	)

NS_END