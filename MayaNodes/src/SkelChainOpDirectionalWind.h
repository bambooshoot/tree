#pragma once

#include <SkelChainOpBase.h>
#include <SkelNoise.h>

NS_BEGIN

class TrunkOpDirectionalWind : public AniOpBase
{
public:
	~TrunkOpDirectionalWind() override {};
	Quat computeQ(CRAniOpState state, CRAniOpData data) const override
	{
		Quat q;
		Float rtRatio = state.u;
		CFloat timeURatio = data.time * data.noiseTrunk[2];
		CFloat timeRatio = data.time * data.noiseTrunk[3];
		if (data.noiseTrunk[1] > 1e-10f)
			rtRatio *= Noise::noise.value(state.u * data.noiseTrunk[1] + timeURatio);

		Vec jointLocAxis(1, 0, 0);
		Vec windDir = data.windDirection;
		state.worldInvMatrix.multDirMatrix(windDir, windDir);

		Vec axis = jointLocAxis.cross(windDir);
		axis.normalize();

		Float noiseValue = Noise::noise.value(timeRatio);

		q.setAxisAngle(axis, noiseValue * state.fTrunkValue * rtRatio);
		return q;
	}
};

class BranchOpDirectionalWind : public AniOpBase
{
public:
	~BranchOpDirectionalWind() override {};
	Quat computeQ(CRAniOpState state, CRAniOpData data) const override
	{
		Quat q;

		Vec axis;
		axis.x = Noise::noise.value(state.p.x);
		axis.y = Noise::noise.value(state.p.y);
		axis.z = Noise::noise.value(state.p.z);

		Random rand(state.uIdx);
		Float randF = rand.nextf(0.0f, data.noiseBranch[2]);

		CFloat noiseValueOffset = state.fIdx * data.noiseBranch[1];

		Float noiseValue = Noise::noise.value(data.time * randF + noiseValueOffset);

		q.setAxisAngle(axis, noiseValue * state.fBranchValue * state.u);
		return q;
	}
};

class FoliageOpDirectionalWind : public AniOpBase
{
public:
	~FoliageOpDirectionalWind() override {};
	Quat computeQ(CRAniOpState state, CRAniOpData data) const override
	{
		Quat q;
		
		Vec axis;
		axis.x = Noise::noise.value(state.p.x);
		axis.y = Noise::noise.value(state.p.y);
		axis.z = Noise::noise.value(state.p.z);

		Random rand(state.uIdx);
		Float randF = rand.nextf(0.0f, data.noiseFoliage[2]);

		Float noiseValueOffset = state.fIdx * data.noiseFoliage[1];

		Float noiseValue = Noise::noise.value(data.time * randF + noiseValueOffset);

		axis.normalize();
		q.setAxisAngle(axis, noiseValue * data.noiseFoliage[0]);
		return q;
	}
};

NS_END