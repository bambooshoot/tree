#include <SkelChain.h>
#include <SkelSpaceFactory.h>

NS_BEGIN

Chain::~Chain()
{
	for (auto pSpace : jointList) {
		DELETE_POINTER(pSpace);
	}
}

void Chain::build(CRChainData chainData, CSkelTreeDataP pTreeData, CMatrix44P pParentMatrix)
{
	SpaceFactory fac;
	jointList.push_back(fac.create(OffsetFrame::typeId, &chainData.offsetFrameData, pTreeData));

	for (auto& frameData : chainData.frameDataList)
		jointList.push_back(fac.create(Frame::typeId, &frameData, pTreeData));

	Matrix44 zeroMat;
	CUint cSpaceNum = Uint(jointList.size()) + 1;
	restMatrixList.resize(cSpaceNum, zeroMat);
	restInvMatrixList.resize(cSpaceNum, zeroMat);
	matrixList.resize(cSpaceNum, zeroMat);
	_updateRestMatrix(pParentMatrix);
	_updateParam();
}

Uint Chain::spaceNum() const
{
	return Uint(matrixList.size());
}

Uint Chain::jointNum() const
{
	return Uint(jointList.size());
}

CRMatrix44 Chain::matrix(CUint spaceId) const
{
	return matrixList[spaceId];
}

CRMatrix44 Chain::restMatrix(CUint spaceId) const
{
	return restMatrixList[spaceId];
}

CRMatrix44 Chain::restInvMatrix(CUint spaceId) const
{
	return restInvMatrixList[spaceId];
}

void Chain::updateMatrix(CRQuatList qList, CMatrix44P pRootMat)
{
	Matrix44 mat;

	auto iter = matrixList.begin();

	if (pRootMat != nullptr)
		*iter = *pRootMat;

	mat = *iter;
	++iter;

	auto qIter = qList.begin();
	for (auto& space : jointList) {
		mat = space->matrix(*qIter) * mat;
		*iter = mat;
		++iter;
		++qIter;
	}
}

Float Chain::xParam(CUint spaceId) const
{
	return xParamList[spaceId];
}

void Chain::_updateRestMatrix(CMatrix44P pRootMat)
{
	Matrix44 mat;

	auto iter = restMatrixList.begin();

	if (pRootMat != nullptr)
		*iter = *pRootMat;

	auto invIter = restInvMatrixList.begin();
	*invIter = iter->inverse();

	mat = *iter;

	++iter;
	++invIter;

	for (auto& space : jointList) {
		mat = space->restMatrix() * mat;
		*iter = mat;
		*invIter = mat.inverse();
		++iter;
		++invIter;
	}
}

void Chain::_updateParam()
{
	if (jointList.size() <= 2) {
		xParamList.resize(jointList.size(), 1.0f);
		return;
	}

	Float x = 0.0f, xLen;

	xParamList.push_back(0);

	for (auto& space : jointList) {
		xLen = space->xParam();
		x += xLen;

		xParamList.push_back(x);
	}

	CFloat xMaxFactor = 1.0f / xParamList.back();

	for (auto& x : xParamList)
		x *= xMaxFactor;
}

VecList Chain::restJointList() const
{
	VecList pList;
	for (auto iter = restMatrixList.begin() + 1; iter != restMatrixList.end();++iter) {
		pList.push_back(iter->translation());
	}
	return pList;
}

CRMatrix44 Chain::jointMatrix(CUint jointId) const
{
	return matrixList[jointId + 1];
}

CRMatrix44 Chain::jointRestMatrix(CUint jointId) const
{
	return restMatrixList[jointId + 1];
}

CRMatrix44 Chain::jointRestInvMatrix(CUint jointId) const
{
	return restInvMatrixList[jointId + 1];
}

CFloat Chain::xLen(CUint boneId) const
{
	return jointList[boneId + 1]->xParam();
}

Uint Chain::boneNum() const
{
	return Uint(jointList.size()) - 1;
}

NS_END