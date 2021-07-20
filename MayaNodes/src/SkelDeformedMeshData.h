#pragma once

#include <SkelTreeBase.h>
#include <SkelSpline.h>

NS_BEGIN

STRUCT(DeformedMeshWeight,
	Uint pId;
SplineWeight<DEFORMED_WEIGHT_NUM> w;)

STRUCT(DeformedMeshData,
	Ushort chainId;
Ushort pointsId;
DeformedMeshWeightList wList;
)

NS_END