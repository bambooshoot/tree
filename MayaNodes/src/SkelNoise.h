#pragma once

#include <SkelTreeBase.h>
#include <SkelSpline.h>

NS_BEGIN

#define NOISE_SAMPLE_NUM 32
#define NOISE_SAMPLE_NUM_FLOAT Float(32)

struct Noise
{
	Noise();
	Float value(CFloat time);

	FloatList noiseSampleList;

	static Noise noise;
};


NS_END