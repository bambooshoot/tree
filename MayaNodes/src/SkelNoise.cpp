#include <SkelNoise.h>

NS_BEGIN

Noise Noise::noise;

Noise::Noise()
{
	Random rnd(0);
	for (Uint i = 0; i < NOISE_SAMPLE_NUM; ++i) {
		float curValue = rnd.nextf(-1.0f, 1.0f);
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

	static CFloatList uList = { 0.0f,1.0f,2.0f,3.0f };

	for (Int i = -1; i < 3; ++i)
		vList.push_back(noiseSampleList[idxList[utime + i]]);

	Spline1D spline(vList, uList);

	Float frac = time - fFlrTime;

	return spline.value(1.0f + frac);
}

NS_END