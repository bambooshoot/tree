#pragma once

#include <SkelChainOpBase.h>

NS_BEGIN

class ChainOpNoise : public ChainOpBase
{
public:
	Quat computeQ(CFloat chainParam, CFloat spaceParam, CFloat time) override
	{
		Quat q;
		q.setAxisAngle(Vec(0, 1, 0), time);
		return q;
	}
};

NS_END