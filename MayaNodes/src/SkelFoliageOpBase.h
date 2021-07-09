#pragma once

#include <SkelFoliageData.h>

NS_BEGIN

STRUCT(FoliageOpData,
	Float time;
Float freq;
Vec   windDirection;
)

CLASS(FoliageOpBase,
public:
	virtual Quat operator()(CRFoliageData foliageData, CRFoliageOpData opData) = 0;
	)

NS_END