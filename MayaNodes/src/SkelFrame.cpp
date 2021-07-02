#include "SkelFrame.h"

NS_BEGIN

SpaceTypeId Frame::typeId(0x03);

SpaceP Frame::create(CVoidP data, CSkelTreeDataP pTreeData)
{
	return new Frame((FrameData*)data, pTreeData);
}

Frame::Frame(CFrameDataP pData, CSkelTreeDataP pTreeData)
{
	_data = pData;
}

Matrix44 Frame::restMatrix() const
{
	Matrix44 mat;
	mat = _data->q.toMatrix44();
	mat[3][0] = _data->xOffset;
	return mat;
}

Matrix44 Frame::matrix(CRQuat q) const
{
	Matrix44 mat;
	Quat q2 = _data->q * q;
	q2.normalize();
	mat = q2.toMatrix44();
	mat[3][0] = _data->xOffset;
	return mat;
}

Float Frame::xParam() const
{
	return _data->xOffset;
}

NS_END