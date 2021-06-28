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

Float Chain::zParam(CUint spaceId) const
{
	return zParamList[spaceId + 1];
}

Float Chain::zLen(CUint spaceId) const
{
	return spaceList[spaceId + 1]->zParam();
}

void Chain::_updateRestMatrix()
{
	Matrix44 mat;
	mat.makeIdentity();
	auto iter = matrixList.begin();
	for (auto& space : spaceList) {
		mat *= space->restMatrix();
		*iter = mat;
		++iter;
	}
}

void Chain::_updateParam()
{
	Float z = 0.0f;
	for (auto& space : spaceList) {
		z += space->zParam();
		zParamList.push_back(z);
	}
	zParamList.push_back(0.0f);
}

NS_END