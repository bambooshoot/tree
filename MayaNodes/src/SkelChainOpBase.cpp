#include <SkelChainOpBase.h>

NS_BEGIN

void ChainOpBase::operator ()(RChainList chainList, CUint chainId, CRChainOpData data)
{
	ChainInternalOpData internalOpData;
	internalOpData.chainParam = Float(chainId);
	RChain chain = chainList[chainId];
	Uint jointNum = chain.jointNum();

	QuatList qList(jointNum);
	for (Uint i = 0; i < jointNum; ++i) {
		internalOpData.worldInvMatrix = chain.jointRestInvMatrix(i);
		internalOpData.u = chain.xParam(i);
		qList[i] = computeQ(internalOpData, data);
	}

	chain.updateMatrix(qList);
}

NS_END