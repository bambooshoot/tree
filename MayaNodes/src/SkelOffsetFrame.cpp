#include <SkelOffsetFrame.h>

NS_BEGIN

SpaceTypeId OffsetFrame::typeId(0x02);

SpaceP OffsetFrame::create(CVoidP pData, CSkelTreeDataP pTreeData)
{
	return new OffsetFrame((OffsetFrameData*)pData, pTreeData);
}

OffsetFrame::OffsetFrame(COffsetFrameDataP pData, CSkelTreeDataP pTreeData)
{
	_data = pData;
}

Matrix44 OffsetFrame::restMatrix() const
{
	Matrix44 mat;
	mat = _data->q.toMatrix44();
	mat[3][0] = _data->offset.x;
	mat[3][1] = _data->offset.y;
	mat[3][2] = _data->offset.z;
	return mat;
}

Matrix44 OffsetFrame::matrix(CRQuat q) const
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

Float OffsetFrame::xParam() const
{
	return 0.0f;
}

CRQuat OffsetFrame::q() const
{
	return _data->q;
}

NS_END