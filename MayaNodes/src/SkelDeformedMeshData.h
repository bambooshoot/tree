#pragma once

#include <SkelTreeBase.h>
#include <SkelSpline.h>

NS_BEGIN

STRUCT(DeformedMeshWeight,
	Uint pId;
SplineWeight<3> w;)

STRUCT(DeformedMeshData,
	Ushort chainId;
Ushort pointsId;
DeformedMeshWeightList wList;
)

NS_END