#pragma once

#include <SkelSpaceData.h>

NS_BEGIN

STRUCT(FoliageData,
	Ushort pointsId;
	AttachedPointData attachPoint;
	Float scale;
Quat q;)

NS_END