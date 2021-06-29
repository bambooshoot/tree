#include <SkelRootFrame.h>

NS_BEGIN

SpaceTypeId RootFrame::typeId(0x02);

SpaceP RootFrame::create(CVoidP pData, CChainDataP pChainData)
{
	return new RootFrame((RootFrameData*)pData, pChainData);
}

RootFrame::RootFrame(CRootFrameDataP pData, CChainDataP pChainData)
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
	return restMatrix() * q.toMatrix44();
}

Float RootFrame::xParam() const
{
	return 0.0f;
}

NS_END