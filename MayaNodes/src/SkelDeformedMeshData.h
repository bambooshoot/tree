#pragma once

#include <SkelTreeBase.h>
#include <SkelSpline.h>

NS_BEGIN

DECL_ALIAS(SplineWeight, DeformedMeshWeight)

STRUCT(DeformedMeshData,
	Ushort chainId;
Ushort pointsId;
DeformedMeshWeightList wList;
)

NS_END