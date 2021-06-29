#include <SkelChain.h>
#include <SkelSpaceFactory.h>

NS_BEGIN

Chain::~Chain()
{
	for (auto* pSpace : spaceList)
		delete pSpace;
}

void Chain::build(CRChainData chainData)
{
	SpaceFactory fac;
	if (chainData.attachedPointData.pointsId != USHORT_MAX)
		spaceList.push_back(fac.create(AttachedPoint::typeId, &chainData.attachedPointData, &chainData));

	spaceList.push_back(fac.create(RootFrame::typeId, &chainData.rootFrameData, &chainData));

	for (auto& frameData : chainData.frameDataList)
		spaceList.push_back(fac.create(Frame::typeId, &frameData, &chainData));

	Matrix44 zeroMat;
	restMatrixList.resize(spaceList.size(), zeroMat);
	matrixList.resize(spaceList.size(), zeroMat);
	_updateRestMatrix();
	_updateParam();
}

Uint Chain::spaceNum() const
{
	return Uint(spaceList.size());
}

CRMatrix44 Chain::matrix(CUint spaceId) const
{
	return matrixList[spaceId];
}

CRMatrix44 Chain::restMatrix(CUint spaceId) const
{
	return restMatrixList[spaceId];
}

void Chain::updateMatrix(CRQuatList qList)
{
	Matrix44 mat;
	mat.makeIdentity();
	auto iter = matrixList.begin();
	auto qIter = qList.begin();
	for (auto& space : spaceList) {
		mat *= space->matrix(*qIter);
		*iter = mat;
		++iter;
		++qIter;
	}
}

Float Chain::xParam(CUint spaceId) const
{
	return xParamList[spaceId + 1];
}

Float Chain::xLen(CUint spaceId) const
{
	return xLenList[spaceId + 1];
}

CSpaceP Chain::space(CUint spaceId) const
{
	return spaceList[spaceId];
}

void Chain::_updateRestMatrix()
{
	Matrix44 mat;
	mat.makeIdentity();
	auto iter = restMatrixList.begin();
	for (auto& space : spaceList) {
		mat = space->restMatrix() * mat;
		*iter = mat;
		++iter;
	}
}

void Chain::_updateParam()
{
	Float x = 0.0f, xLen;

	for (auto& space : spaceList) {
		xLen = space->xParam();
		x += xLen;

		xLenList.push_back(xLen);
		xParamList.push_back(x);
	}

	xLenList.push_back(0.0f);
	xParamList.push_back(x);
}

NS_END