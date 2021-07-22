#pragma once

#include <SkelTreeBase.h>
#include <SkelSpline.h>

NS_BEGIN

#define NOISE_SAMPLE_NUM 32
#define NOISE_SAMPLE_NUM_LESS_ONE (NOISE_SAMPLE_NUM - 1)
#define NOISE_SAMPLE_NUM_LESS_TWO (NOISE_SAMPLE_NUM - 2)
#define NOISE_SAMPLE_NUM_LESS_FOUR (NOISE_SAMPLE_NUM - 4)
#define NOISE_SAMPLE_NUM_FLOAT Float(NOISE_SAMPLE_NUM)

struct Noise
{
	Noise();
	Float value(CFloat time);

	FloatList noiseSampleList;
	UintList idxList;
	static Noise noise;
};


NS_END