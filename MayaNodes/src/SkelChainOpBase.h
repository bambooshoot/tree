#pragma once

#include <SkelChain.h>

NS_BEGIN

CLASS(ChainOpBase,
public:
	virtual Quat computeQ(CFloat chainParam, CFloat spaceParam, CFloat time) = 0;
	void operator ()(RChain chain, CUint chainId, CFloat time)
	{
		Uint spaceNum = chain.spaceNum();
		CFloat chainParam = Float(chainId);
		QuatList qList(spaceNum);
		for (Uint i = 0; i < spaceNum; ++i) {
			qList[i] = computeQ(chainParam, chain.xParam(i), time);
		}
		
		chain.updateMatrix(qList);
	})

NS_END