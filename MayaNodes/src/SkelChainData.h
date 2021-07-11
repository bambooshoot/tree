#pragma once

#include <SkelSpaceData.h>

NS_BEGIN

STRUCT(ChainData,
	Ushort			trunkFrameId;
	OffsetFrameData	offsetFrameData;
	FrameDataList	frameDataList;
	)

NS_END