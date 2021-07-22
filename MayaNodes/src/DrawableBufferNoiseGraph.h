#pragma once

#include <DrawableBufferBase.h>
#include <SkelNoise.h>

#define NOISE_GRAPH_SAMPLE_LENGTH				(NOISE_SAMPLE_NUM * 3)
#define NOISE_GRAPH_SAMPLE_NUM(SEGNUM)			NOISE_GRAPH_SAMPLE_LENGTH*SEGNUM
#define NOISE_GRAPH_SAMPLE_STEP(SEGNUM)			(1.0f/float(SEGNUM))
#define NOISE_GRAPH_SAMPLE_ORG_X(SEGNUM)		-(NOISE_GRAPH_SAMPLE_NUM(SEGNUM) / 2) * NOISE_GRAPH_SAMPLE_STEP(SEGNUM)

class DrawableBufferNoiseGraph : public DrawableBufferBase
{
public:
	~DrawableBufferNoiseGraph() override {}
	bool hasInstance() const override
	{
		return false;
	}

protected:
	bool hasNormal() const override
	{
		return false;
	}
	bool hasColor() const override
	{
		return true;
	}
	uint vertexNum(DrawableBufferParam& param) const override
	{
		return NOISE_GRAPH_SAMPLE_NUM(param.pPopGeoData->segmentNum) * 3 + NOISE_SAMPLE_NUM;
	}
	uint indexNum(DrawableBufferParam& param) const override
	{
		return NOISE_GRAPH_SAMPLE_NUM(param.pPopGeoData->segmentNum) * 3 + NOISE_SAMPLE_NUM;
	}
	void fillPositions(float* buf, DrawableBufferParam& param) override
	{
		uint idx = 0;

		const float xOrg = NOISE_GRAPH_SAMPLE_ORG_X(param.pPopGeoData->segmentNum);
		const int sampleNum = NOISE_GRAPH_SAMPLE_NUM(param.pPopGeoData->segmentNum);
		const float sampleStep = NOISE_GRAPH_SAMPLE_STEP(param.pPopGeoData->segmentNum);
		const float scale = param.pPopGeoData->noiseGraphScale;
		const float matchStep = sampleStep * scale;

		// trunk noise
		auto noisePosition = [&](skelTree::CRAniOpData param, std::function<float(skelTree::CRAniOpData, float)>& func) {
			float xSample = xOrg;
			float xMatch = xOrg * scale;
			for (int i = 0; i < sampleNum; ++i) {
				buf[idx++] = xMatch;
				buf[idx++] = func(param, xSample) * scale;
				buf[idx++] = 0;

				xSample += sampleStep;
				xMatch += matchStep;
			}
		};

		auto trunkNoiseFunc = [&]() -> std::function<float(skelTree::CRAniOpData, float)> {
			return [&](skelTree::CRAniOpData param, float inX) -> float {
				float x = inX * param.noiseTrunk[3];
				return param.noiseTrunk[0] * skelTree::Noise::noise.value(x);
			};
		};

		noisePosition(*param.pAniOpData, trunkNoiseFunc());

		auto branchNoiseFunc = [&]() -> std::function<float(skelTree::CRAniOpData, float)> {
			return [&](skelTree::CRAniOpData param, float inX) -> float {
				float x = inX * param.noiseBranch[2];
				return param.noiseBranch[0] * skelTree::Noise::noise.value(x) + 2;
			};
		};

		noisePosition(*param.pAniOpData, branchNoiseFunc());

		auto foliageNoiseFunc = [&]() -> std::function<float(skelTree::CRAniOpData, float)> {
			return [&](skelTree::CRAniOpData param, float inX) -> float {
				float x = inX * param.noiseFoliage[2];
				return param.noiseFoliage[0] * skelTree::Noise::noise.value(x) + 4;
			};
		};

		noisePosition(*param.pAniOpData, foliageNoiseFunc());

		const float noiseSampleStep = scale;
		float xMatch = 0;

		for (float f : skelTree::Noise::noise.noiseSampleList) {
			buf[idx++] = xMatch;
			buf[idx++] = (f + 4) * scale;
			buf[idx++] = 0;

			xMatch += noiseSampleStep;
		}
	}
	void fillColors(float* buf, DrawableBufferParam& param) override
	{
		const int sampleNum = NOISE_GRAPH_SAMPLE_NUM(param.pPopGeoData->segmentNum);
		const float sampleStep = NOISE_GRAPH_SAMPLE_STEP(param.pPopGeoData->segmentNum);
		const int clrSize = sizeof(float) * 4;

		uint idx = 0;
		for (uint gId = 0; gId < 3; ++gId) {
			for (uint i = 0; i < NOISE_GRAPH_SAMPLE_LENGTH; ++i) {
				float ratio = 0;
				for (int j = 0; j < param.pPopGeoData->segmentNum; ++j) {
					buf[idx    ] = 0;
					buf[idx + 1] = 0;
					buf[idx + 2] = 0;
					buf[idx + 3] = 1;

					buf[idx + gId] = ratio;
					
					idx += 4;
					ratio += sampleStep;
				}
			}
		}

		for (uint i = 0; i < NOISE_SAMPLE_NUM; ++i) {
			buf[idx++] = 1;
			buf[idx++] = 1;
			buf[idx++] = 1;
			buf[idx++] = 1;
		}
	}
	void fillIndices(unsigned int* indices, DrawableBufferParam& param) override
	{
		const int sampleNum = NOISE_GRAPH_SAMPLE_NUM(param.pPopGeoData->segmentNum);
		uint idx = 0, beginId = 0;
		for (uint gId = 0; gId < 3; ++gId) {
			for (int i = 0; i < sampleNum; ++i) {
				indices[idx++] = idx;
			}
			//for (uint i = 0; i < sampleNum - 1; ++i) {
			//	indices[idx++] = i + beginId;
			//	indices[idx++] = i + 1 + beginId;
			//}
			//beginId += sampleNum;
		}

		for (uint i = 0; i < NOISE_SAMPLE_NUM; ++i)
			indices[idx++] = idx;
	}
};