#pragma once

#include <SkelTreeBase.h>
#include <SkelPoints.h>

NS_BEGIN

STRUCT(AttachedPointData,
	Ushort	pointsId;
	Uint	vid[3];
	Float	w[2];
	)

STRUCT(RootFrameData,
	Vec		offset;
	Quat	q;
	)

STRUCT(FrameData,
	Float xOffset;
	Quat q;
	)

NS_END