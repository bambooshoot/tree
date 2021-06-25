#include <SkelDeformedMesh.h>

NS_BEGIN

void DeformedMesh::computeWeights()
{
	RDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->attrList[Points::AttrType::Rest];
	
	wList.resize(restPoints.size());
	auto pIter = restPoints.begin();
	for (auto& w : wList) {
		Uint nearestId = _nearestSpace(*pIter);
		_setWeight(w, *pIter, nearestId);
		++pIter;
	}

	pPoints->addDeformed();
}

void DeformedMesh::deform()
{
	CRChain chain = *pChain;
	CRDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->attrList[Points::AttrType::Rest];
	RVecList  deformedPoints = pPoints->attrList[Points::AttrType::Deformed];

	auto rIter = restPoints.begin();
	auto dIter = deformedPoints.begin();
	for (auto& w : wList) {
		*dIter = *rIter * w.w[0] * chain.matrix(w.spaceId[0]) + *rIter * w.w[1] * chain.matrix(w.spaceId[1]);
		++dIter;
		++rIter;
	}
}

void DeformedMesh::_setWeight(RDeformedMeshWeight w, CRVec p, CUint id)
{
	CRChain chain = *pChain;
	Vec locP;
	Float zLen;
	_locPAndZLen(locP, zLen, p, id);

	if (locP.z >= 0 && locP.z <= zLen) {
		w.spaceId[0] = id; w.spaceId[1] = id + 1;
		w.w[1] = locP.z / zLen;
		w.w[0] = 1.0f - w.w[1];
		return;
	}

	// p is out of space chain, below the first space or above the last space.
	w.spaceId[0] = id; w.spaceId[1] = id;
	w.w[0] = 1.0f; w.w[1] = 0.0f;
}

void DeformedMesh::_locPAndZLen(RVec locP, RFloat zLen, CRVec p, CUint id) const
{
	CRChain chain = *pChain;
	CRMatrix44 mat = chain.restMatrix(id);
	Matrix44 invMat = mat.inverse();
	locP = p * invMat;
	zLen = chain.zLen(id);
}

Uint DeformedMesh::_nearestSpace(CRVec p) const
{
	CRChain chain = *pChain;
	FloatList zList, r2List, zlList;
	Vec locP;
	Float zLen;
	for (Uint i = 0; i < chain.spaceNum(); ++i) {
		_locPAndZLen(locP, zLen, p, i);
		zList.push_back(locP.z);
		r2List.push_back(locP.x * locP.x + locP.y * locP.y);
		zlList.push_back(zLen);
	}

	UintList idList;
	auto zIter = zList.begin();
	auto rIter = r2List.begin();
	auto zlIter = zlList.begin();
	for (Uint i = 0; i < zList.size(); ++i, ++zIter, ++rIter, ++zlIter) {
		if (*zIter > 0 && *zIter < *zlIter)
			idList.push_back(i);
	}

	if (idList.size() > 0) {
		Float nearestR = 1000.0f;
		Uint nearestId = 0;
		for (Uint id : idList) {
			if (nearestR > r2List[id]) {
				nearestR = r2List[id];
				nearestId = id;
			}
		}

		return nearestId;
	}

	Float nearestZ = 1000.0f;
	Float absZ;
	Uint nearestId = 0;
	Uint i = 0;
	for (zlIter = zlList.begin(); zlIter != zlList.end(); ++zlIter, ++i) {
		absZ = abs(*zlIter);
		if (nearestZ > absZ) {
			nearestZ = absZ;
			nearestId = i;
		}
	}
	return nearestId;
}

NS_END