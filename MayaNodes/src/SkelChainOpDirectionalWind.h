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

		CVec noiseValueOffset = state.p * data.noiseBranch[1];
		CFloat timeValue = data.time * data.noiseBranch[2];

		axis.x = 0.0f;
		axis.y = 1.0f;
		axis.z = 0.0f;

		Float noiseValue = Noise::noise.value(timeValue + noiseValueOffset.x)
			* Noise::noise.value(timeValue + noiseValueOffset.y)
			* Noise::noise.value(timeValue + noiseValueOffset.z);

		//axis.normalize();
		q.setAxisAngle(axis, noiseValue * state.fBranchValue);
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

		Float timeValue = data.time * data.noiseFoliage[2];

		axis.x = 1.0f;
		axis.y = 0.0f;
		axis.z = 0.0f;

		Random rand(state.uIdx);
		CFloat randF = rand.nextf() * data.noiseFoliage[3];

		CRVec windDir = data.windDirection.normalized();
		CVec windZDir = windDir.cross(CVec(0.0f, 1.0f, 0.0f)).normalized();
		CVec windYDir = windZDir.cross(windDir).normalized();

		Vec noiseValueOffset(
			state.p.dot(windDir),
			state.p.dot(windYDir),
			state.p.dot(windZDir)
		);

		noiseValueOffset *= data.noiseFoliage[1];

		noiseValueOffset.x += timeValue + randF;

		Float noiseValue = Noise::noise.value(noiseValueOffset.x)
			* Noise::noise.value(noiseValueOffset.y)
			* Noise::noise.value(noiseValueOffset.z);

		//axis.normalize();
		q.setAxisAngle(axis, noiseValue * data.noiseFoliage[0]);
		return q;
	}
};

NS_END