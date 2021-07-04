#pragma once

#include <SkelSpace.h>
#include <SkelTreeData.h>

NS_BEGIN

CLASSDERIVED(Frame, Space,
public:
	Frame(CFrameDataP pData, CSkelTreeDataP pTreeData);
	~Frame() override = default;
	Matrix44 matrix(CRQuat q) const override;
	Matrix44 restMatrix() const override;
	Float xParam() const override;
	CRQuat q() const override;

	static SpaceP create(CVoidP data, CSkelTreeDataP pTreeData);
	static SpaceTypeId typeId;

private:
	CFrameDataP _data;
	)

NS_END