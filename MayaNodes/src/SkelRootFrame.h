#pragma once

#include <SkelSpace.h>
#include <SkelChainData.h>

NS_BEGIN

CLASSDERIVED(RootFrame, Space,
public:
	RootFrame(CRootFrameDataP pData, CChainDataP pChainData);
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float zParam() const override;

	static SpaceP create(CVoidP data, CChainDataP pChainData);
	static SpaceTypeId typeId;

private:
	CRootFrameDataP _data;
	)

NS_END