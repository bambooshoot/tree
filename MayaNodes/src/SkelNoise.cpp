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
}

Float Noise::value(CFloat time)
{
	FloatList vList, uList;
	Uint utime = Uint(time) % NOISE_SAMPLE_NUM;
	Uint utime_p = (utime - 1) % NOISE_SAMPLE_NUM;
	Uint utime_n = (utime + 1) % NOISE_SAMPLE_NUM;

	Float timeCutOff = Float(utime);
	Float timeCutOff_P = Float(utime_p);
	Float timeCutOff_N = Float(utime_n);

	uList.push_back(timeCutOff_P);
	uList.push_back(timeCutOff);
	uList.push_back(timeCutOff_N);

	vList.push_back(noiseSampleList[utime_p]);
	vList.push_back(noiseSampleList[utime]);
	vList.push_back(noiseSampleList[utime_n]);

	Spline1D spline(vList, uList);

	Float locTime = fmodf(time, NOISE_SAMPLE_NUM_FLOAT);

	return spline.value(locTime);
}

NS_END