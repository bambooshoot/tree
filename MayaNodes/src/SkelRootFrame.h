#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

NS_BEGIN

CLASSDERIVED(RootFrame, Space,
public:
	~RootFrame() override {};
	RootFrame(CRootFrameDataP pData, CSkelTreeDataP pTreeData);
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float xParam() const override;

	static SpaceP create(CVoidP data, CSkelTreeDataP pTreeData);
	static SpaceTypeId typeId;

private:
	CRootFrameDataP _data;
	)

NS_END