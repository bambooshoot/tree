#include <SkelDeformedMesh.h>
#include <SkelSpline.h>
#include <iostream>

NS_BEGIN

void DeformedMesh::computeWeights()
{
	RDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->rest();
	Spline3D* spline = Spline3D::create(pChain->restJointList());

	for (auto& w : wList) {
		spline->weights(w.w, restPoints[w.pId]);
	}

	DELETE_POINTER(spline)

	pPoints->addDeformed();
}

void DeformedMesh::deform()
{
	CRChain chain = *pChain;
	CRDeformedMeshWeightList wList = pDeformedMeshData->wList;

	pPoints->addDeformed();

	CRVecList restPoints = pPoints->rest();
	RVecList  deformedPoints = pPoints->deformed();

	for (auto& w : wList) {
		auto dIter = deformedPoints.begin() + w.pId;
		auto rIter = restPoints.begin() + w.pId;
		*dIter = Vec(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < SPLINE_WEIGHT_NUM; ++i) {
			CRMatrix44 restInvMat = chain.jointRestInvMatrix(w.w.id[i]);
			CRMatrix44 mat = chain.jointMatrix(w.w.id[i]);

			*dIter += ((*rIter * restInvMat) * mat) * w.w.w[i];
		}
	}
}

NS_END