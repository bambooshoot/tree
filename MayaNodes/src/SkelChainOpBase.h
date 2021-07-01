#pragma once

#include <SkelChain.h>

NS_BEGIN

CLASS(ChainOpBase,
public:
	virtual Quat computeQ(CFloat chainParam, CFloat spaceParam, CFloat time, CFloat value) = 0;
	void operator ()(RChainList chainList, CFloat time, CFloat value)
	{
		for (Uint chainId = 0; chainId < chainList.size(); ++chainId) {
			CFloat chainParam = Float(chainId);
			RChain chain = chainList[chainId];
			Uint spaceNum = chain.spaceNum();
			QuatList qList(spaceNum);
			for (Uint i = 0; i < spaceNum; ++i) {
				qList[i] = computeQ(chainParam, chain.xParam(i), time, value);
			}

			chain.updateMatrix(qList);
		}
	})

NS_END