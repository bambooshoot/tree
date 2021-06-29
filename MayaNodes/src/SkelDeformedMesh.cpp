#include <SkelDeformedMesh.h>

NS_BEGIN

void DeformedMesh::computeWeights()
{
	RDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->rest();
	
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
	CRVecList restPoints = pPoints->rest();
	RVecList  deformedPoints = pPoints->deformed();

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
	Float xLen;
	_locPAndXLen(locP, xLen, p, id);

	if (locP.x >= 0 && locP.x <= xLen) {
		w.spaceId[0] = id; w.spaceId[1] = id + 1;
		w.w[1] = locP.x / xLen;
		w.w[0] = 1.0f - w.w[1];
		return;
	}

	// p is out of space chain, below the first space or above the last space.
	w.spaceId[0] = id; w.spaceId[1] = id;
	w.w[0] = 1.0f; w.w[1] = 0.0f;
}

void DeformedMesh::_locPAndXLen(RVec locP, RFloat zLen, CRVec p, CUint id) const
{
	CRChain chain = *pChain;
	CRMatrix44 mat = chain.restMatrix(id);
	Matrix44 invMat = mat.inverse();
	locP = p * invMat;
	zLen = chain.xLen(id);
}

Uint DeformedMesh::_nearestSpace(CRVec p) const
{
	CRChain chain = *pChain;
	FloatList xList, r2List, xlList;
	Vec locP;
	Float xLen;
	for (Uint i = 0; i < chain.spaceNum(); ++i) {
		_locPAndXLen(locP, xLen, p, i);
		xList.push_back(locP.x);
		r2List.push_back(locP.z * locP.z + locP.y * locP.y);
		xlList.push_back(xLen);
	}

	UintList idList;
	auto xIter = xList.begin();
	auto xlIter = xlList.begin();
	for (Uint i = 0; i < xList.size(); ++i, ++xIter, ++xlIter) {
		if (*xIter > 0 && *xIter < *xlIter)
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

	Float nearestX = 1000.0f;
	Float absX;
	Uint nearestId = 0;
	Uint i = 0;
	for (xlIter = xlList.begin(); xlIter != xlList.end(); ++xlIter, ++i) {
		absX = abs(*xlIter);
		if (nearestX > absX) {
			nearestX = absX;
			nearestId = i;
		}
	}
	return nearestId;
}

NS_END