#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

NS_BEGIN

CLASSDERIVED(OffsetFrame, Space,
public:
	OffsetFrame(COffsetFrameDataP pData, CSkelTreeDataP pTreeData);
	~OffsetFrame() override = default;
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float xParam() const override;
	CRQuat q() const override;

	static SpaceP create(CVoidP data, CSkelTreeDataP pTreeData);
	static SpaceTypeId typeId;

private:
	COffsetFrameDataP _data;
	)

NS_END