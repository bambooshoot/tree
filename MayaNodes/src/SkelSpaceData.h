#pragma once

#include <SkelTreeBase.h>
#include <SkelPoints.h>

NS_BEGIN

STRUCT(AttachedWeight,
	Uint	vid;
	Float	w;
)

STRUCT(AttachedPointData,
	Ushort					pointsId;
	AttachedWeightList		wList;
	)

STRUCT(RootFrameData,
	Vec		offset;
	Quat	q;
	)

STRUCT(FrameData,
	Float zOffset;
	Quat q;
	)

NS_END