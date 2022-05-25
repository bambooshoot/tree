#include <SkelNoise.h>

NS_BEGIN

Noise Noise::noise;

Noise::Noise()
{
	Random rnd(0);
	auto Valid = [&](CFloat value, CUint i) {
		const float bound = 0.15f, bound2 = 0.3f;
		if (i % 2 == 1)
			return value > -bound && value < bound;

		if ((i / 2) % 2 == 0)
			return value < -bound2;

		return value > bound2;
	};
	for (Uint i = 0; i < NOISE_SAMPLE_NUM; ++i) {
		float curValue;
		do {
			curValue = rnd.nextf(-1.0f, 1.0f);
		} while (!Valid(curValue, i));

		noiseSampleList.push_back(curValue);
	}

	idxList.push_back(NOISE_SAMPLE_NUM_LESS_ONE);
	for (Uint i = 0; i < NOISE_SAMPLE_NUM; ++i) {
		idxList.push_back(i);
	}
	idxList.push_back(0);
	idxList.push_back(1);
}

Float Noise::value(CFloat time)
{
	FloatList vList;
	
	Float fFlrTime = floor(time);
	Int utime = Int(fFlrTime) % NOISE_SAMPLE_NUM;

	if (utime < 0)
		utime = NOISE_SAMPLE_NUM + utime;

	++utime;

	FloatList uList;
	Int idx;
	for (Int i = -1; i < 3; ++i) {
		idx = utime + i;
		vList.push_back(noiseSampleList[idxList[idx]]);
		uList.push_back(fFlrTime + float(i));
	}

	Spline1D spline(vList, uList);
	return spline.value(time);
}

NS_END