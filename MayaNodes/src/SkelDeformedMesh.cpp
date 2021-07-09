#include <SkelDeformedMesh.h>
#include <SkelSpline.h>

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
	CRVecList restPoints = pPoints->rest();
	RVecList  deformedPoints = pPoints->deformed();

	auto rIter = restPoints.begin();
	auto dIter = deformedPoints.begin();

	for (auto& w : wList) {
		*dIter = Vec(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < SPLINE_WEIGHT_NUM; ++i) {
			CRMatrix44 restInvMat = chain.jointRestInvMatrix(w.w.id[i]);
			CRMatrix44 mat = chain.jointMatrix(w.w.id[i]);

			*dIter += ((*rIter * restInvMat) * mat) * w.w.w[i];
		}
		++dIter;
		++rIter;
	}
}

NS_END