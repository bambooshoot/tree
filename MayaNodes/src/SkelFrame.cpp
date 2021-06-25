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
	mat.setTranslation(Vec(0.0f, 0.0f, _data->zOffset));
	mat *= _data->q.toMatrix44();
	return mat;
}

Matrix44 Frame::matrix(CRQuat q) const
{
	return restMatrix() * q.toMatrix44();
}

Float Frame::zParam() const
{
	return _data->zOffset;
}

NS_END