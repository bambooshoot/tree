#pragma once

#include <SkelChain.h>

NS_BEGIN

STRUCT(AniOpData,
Vec windDirection;
Float time;
Float noiseTrunk[4];
Float noiseBranch[3];
Float noiseFoliage[4];
void offsetTime(CFloat offset)
{
	time += offset;
}
void scaleTime()
{
	time *= 0.05f;
}
)

STRUCT(AniOpState,
Matrix44	worldInvMatrix;
Float		u;
Vec			p;
Float		fIdx;
Uint        uIdx;
Float       fSpaceNum;
Float       fBranchValue;
Float       fTrunkValue;
)

CLASS(AniOpBase,
public:
	virtual ~AniOpBase() {};
	virtual Quat computeQ(CRAniOpState state, CRAniOpData data) const = 0;
	QuatList chainOp(RChain chain, CUint chainId, CRAniOpData data);
	)

NS_END