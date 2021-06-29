#include "SkelFrame.h"

NS_BEGIN

SpaceTypeId Frame::typeId(0x03);

SpaceP Frame::create(CVoidP data, CChainDataP pChainData)
{
	return new Frame((FrameData*)data, pChainData);
}

Frame::Frame(CFrameDataP pData, CChainDataP pChainData)
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
	return restMatrix() * q.toMatrix44();
}

Float Frame::xParam() const
{
	return _data->xOffset;
}

NS_END