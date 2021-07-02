#include <SkelRootFrame.h>

NS_BEGIN

SpaceTypeId RootFrame::typeId(0x02);

SpaceP RootFrame::create(CVoidP pData, CSkelTreeDataP pTreeData)
{
	return new RootFrame((RootFrameData*)pData, pTreeData);
}

RootFrame::RootFrame(CRootFrameDataP pData, CSkelTreeDataP pTreeData)
{
	_data = pData;
}

Matrix44 RootFrame::restMatrix() const
{
	Matrix44 mat;
	mat = _data->q.toMatrix44();
	mat[3][0] = _data->offset.x;
	mat[3][1] = _data->offset.y;
	mat[3][2] = _data->offset.z;
	return mat;
}

Matrix44 RootFrame::matrix(CRQuat q) const
{
	Matrix44 mat;
	Quat q2 = _data->q * q;
	q2.normalize();
	mat = q2.toMatrix44();
	mat[3][0] = _data->offset.x;
	mat[3][1] = _data->offset.y;
	mat[3][2] = _data->offset.z;
	return mat;
}

Float RootFrame::xParam() const
{
	return 0.0f;
}

NS_END