#pragma once

#include <SkelChain.h>

NS_BEGIN

STRUCT(ChainOpData,
Vec windDirection;
Float time;
Float scale;
Float offset;
Float freqU;
Float freqChain;
)

STRUCT(ChainInternalOpData,
Matrix44 worldInvMatrix;
Float chainParam;
Float u;
)

CLASS(ChainOpBase,
public:
	virtual Quat computeQ(CRChainInternalOpData internalOpData, CRChainOpData data) = 0;
	void operator ()(RChainList chainList, CUint chainId, CRChainOpData data);
	)

NS_END