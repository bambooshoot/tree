#pragma once

#include <SkelChainOpBase.h>
#include <SkelNoise.h>

NS_BEGIN

class ChainOpDirectionalWind : public ChainOpBase
{
public:
	Quat computeQ(CRChainInternalOpData internalOpData, CRChainOpData data) override
	{
		Quat q;
		Float rtRatio = internalOpData.u * data.freqU;
		Float chainParam = internalOpData.chainParam * data.freqChain;

		Vec jointLocAxis(1, 0, 0);
		Vec windDir = data.windDirection;
		internalOpData.worldInvMatrix.multDirMatrix(windDir, windDir);

		Vec axis = jointLocAxis.cross(windDir);
		axis.normalize();

		Float noiseValue = Noise::noise.value(data.time + rtRatio + chainParam) + data.offset;

		q.setAxisAngle(axis, noiseValue * data.scale * rtRatio);
		return q;
	}
};

NS_END