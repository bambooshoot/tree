#pragma once

#include <SkelSpace.h>
#include <SkelChainData.h>

NS_BEGIN

CLASSDERIVED(Frame, Space,
public:
	Frame(CFrameDataP pData, CChainDataP pChainData);
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float zParam() const override;

	static SpaceP create(CVoidP data, CChainDataP pChainData);
	static SpaceTypeId typeId;

private:
	CFrameDataP _data;
	)

NS_END