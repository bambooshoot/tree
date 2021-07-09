#include <SkelGeoBox.h>

NS_BEGIN

constexpr CBoxPoints8 initBoxPoints8()
{
	BoxPoints8 boxPnts = { 0 };
	int idx = 0;
	const float x2[2] = { 0.0f, 1.0f };
	const float y2[2] = { -1.0f,1.0f };
	const float z2[2] = { -1.0f,1.0f };
	for (int i = 0; i < 2; ++i) {
		for (int j = 0; j < 2; ++j) {
			for (int k = 0; k < 2; ++k) {
				boxPnts[idx++] = x2[k];
				boxPnts[idx++] = y2[j];
				boxPnts[idx++] = z2[i];
			}
		}
	}
	return boxPnts;
}

constexpr CBoxPoints6Sides initBoxPoints6Sides()
{
	CBoxPoints8 pnts = initBoxPoints8();

	const int vtxIds[3][8] = {
		{0,2,4,6,1,3,5,7},
		{0,1,2,3,4,5,6,7},
		{0,1,4,5,2,3,6,7}
	};

	BoxPoints6Sides vtxs = {0};

	int idx = 0;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 8; ++j) {
			int pId3 = vtxIds[i][j] * 3;
			for (int k = 0; k < 3; ++k)
				vtxs[idx++] = pnts[pId3 + k];
		}
	}

	return vtxs;
}

constexpr CBoxColors initBoxColors()
{
	BoxColors boxClrs = { 0 };

	int idx = 0;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 8; ++j, idx += 4) {
			for (int k = 0; k < 3; ++k)
				boxClrs[idx + k] = 0;
			
			boxClrs[idx + i] = 1.0f;
			boxClrs[idx + 3] = 1.0f;
		}
	}

	return boxClrs;
}

constexpr CBoxTriIndices initTriIndices()
{
	BoxTriIndices triIdx = { 0 };

	int vtxBeginId = 0, idx = 0;
	for (int i = 0; i < 6; ++i, vtxBeginId += 4) {
		triIdx[idx++] = vtxBeginId;
		triIdx[idx++] = 3 + vtxBeginId;
		triIdx[idx++] = 1 + vtxBeginId;

		triIdx[idx++] = vtxBeginId;
		triIdx[idx++] = 2 + vtxBeginId;
		triIdx[idx++] = 3 + vtxBeginId;
	}

	return triIdx;
}

constexpr CBoxWireframeIndices initWireframeIndices()
{
	BoxWireframeIndices wireIdx = { 0 };
	int idx = 0, ii = 0;
	for (int i = 0; i < 4; ++i, ii += 2) {
		wireIdx[idx++] = ii;
		wireIdx[idx++] = ii + 1;
	}

	for (int i = 0; i < 4; ++i) {
		wireIdx[idx++] = i;
		wireIdx[idx++] = i + 4;
	}

	const int sidx[4] = { 0,1,4,5 };
	for (int i = 0; i < 4; ++i) {
		wireIdx[idx++] = sidx[i];
		wireIdx[idx++] = sidx[i] + 2;
	}

	return wireIdx;
}

CBoxPoints6Sides		GeoBox::points = initBoxPoints6Sides();
CBoxColors				GeoBox::colors = initBoxColors();
CBoxTriIndices			GeoBox::triIndices = initTriIndices();
CBoxWireframeIndices	GeoBox::wireframeIndices = initWireframeIndices();

NS_END