#include <SkelDeformedMesh.h>
#include <SkelSpline.h>
#include <iostream>

#include <omp.h>

NS_BEGIN

void DeformedMesh::computeWeights()
{
	RDeformedMeshWeightList wList = pDeformedMeshData->wList;
	CRVecList restPoints = pPoints->rest();
	Spline3D2P* spline = Spline3D2P::create(pChain->restJointList());

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

	CInt wListNum = (CInt)wList.size();
#pragma omp parallel for
	for (Int k = 0; k < wListNum; ++k) {
		auto& w = wList[k];
		auto dIter = deformedPoints.begin() + w.pId;
		auto rIter = restPoints.begin() + w.pId;
		*dIter = Vec(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < DEFORMED_WEIGHT_NUM; ++i) {
			CRMatrix44 deformMat = chain.jointDeformMatrix(w.w.id[i]);
			*dIter += (*rIter * deformMat) * w.w.w[i];
		}
	}
}

NS_END