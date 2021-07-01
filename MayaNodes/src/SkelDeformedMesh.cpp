#include <SkelDeformedMesh.h>
#include <SkelSpline.h>

NS_BEGIN

void DeformedMesh::computeWeights()
{
	RDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->rest();
	Spline3D spline(pChain->restPointList());

	wList.resize(restPoints.size());
	auto pIter = restPoints.begin();
	for (auto& w : wList) {
		spline.weights(w, *pIter);
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
		*dIter = Vec(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < SPLINE_WEIGHT_NUM; ++i) {
			CRMatrix44 restInvMat = chain.restInvMatrix(w.id[i]);
			CRMatrix44 mat = chain.matrix(w.id[i]);

			*dIter += ((*rIter * restInvMat) * mat) * w.w[i];
		}
		++dIter;
		++rIter;
	}
}

NS_END