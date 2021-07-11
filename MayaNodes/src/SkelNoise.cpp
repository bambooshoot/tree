#include <SkelNoise.h>

NS_BEGIN

Noise Noise::noise;

Noise::Noise()
{
	Random rnd(0);
	for (Uint i = 0; i < NOISE_SAMPLE_NUM_LESS_ONE; ++i) {
		float curValue = rnd.nextf(-1.0f, 1.0f);
		noiseSampleList.push_back(curValue);
	}
	noiseSampleList.push_back(noiseSampleList[0]);
}

Float Noise::value(CFloat time)
{
	FloatList vList, uList;
	Uint utime = 2 + Uint(time) % NOISE_SAMPLE_NUM_LESS_FOUR;
	Uint utime_p1 = utime - 2;
	Uint utime_p2 = utime - 1;
	Uint utime_n1 = utime + 1;
	Uint utime_n2 = utime + 2;

	uList.push_back(Float(utime_p1));
	uList.push_back(Float(utime_p2));
	uList.push_back(Float(utime));
	uList.push_back(Float(utime_n1));
	uList.push_back(Float(utime_n2));

	vList.push_back(noiseSampleList[utime_p1]);
	vList.push_back(noiseSampleList[utime_p2]);
	vList.push_back(noiseSampleList[utime]);
	vList.push_back(noiseSampleList[utime_n1]);
	vList.push_back(noiseSampleList[utime_n2]);

	Spline1D spline(vList, uList);

	Float locTime = utime + (time - floorf(time));

	return spline.value(locTime);
}

NS_END