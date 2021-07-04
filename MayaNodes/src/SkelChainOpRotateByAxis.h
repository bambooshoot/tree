#pragma once

#include <SkelChainOpBase.h>
#include <SkelNoise.h>

NS_BEGIN

class ChainOpRotateByAxis : public ChainOpBase
{
public:
	Quat computeQ(CRChainInternalOpData internalOpData, CRChainOpData data) override
	{
		Quat q;
		Float rtRatio = internalOpData.u;

		Vec axis = data.windDirection;
		internalOpData.worldInvMatrix.multDirMatrix(axis, axis);

		Float noiseValue = Noise::noise.value(data.time);

		q.setAxisAngle(axis, noiseValue * data.scale * rtRatio);
		return q;
	}
};

NS_END