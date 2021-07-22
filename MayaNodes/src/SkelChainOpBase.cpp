#include <SkelChainOpBase.h>

NS_BEGIN

QuatList AniOpBase::chainOp(RChain chain, CUint chainId, CRAniOpData data)
{
	AniOpState internalOpData;
	CUint jointNum = chain.jointNum();

	internalOpData.uIdx = chainId;
	internalOpData.fIdx = Float(chainId);
	internalOpData.fSpaceNum = Float(jointNum);
	CFloat fInvSpaceNum = 1.0f / internalOpData.fSpaceNum;
	internalOpData.fBranchValue = data.noiseBranch[0] * fInvSpaceNum;
	internalOpData.fTrunkValue = data.noiseTrunk[0] * fInvSpaceNum;

	QuatList qList(jointNum);
	for (Uint i = 0; i < jointNum; ++i) {
		internalOpData.worldInvMatrix = chain.jointRestInvMatrix(i);
		internalOpData.u = chain.xParam(i);
		CRMatrix44 mat = chain.jointMatrix(i);
		internalOpData.p = mat.translation();
		
		qList[i] = computeQ(internalOpData, data);
	}

	return qList;
}

NS_END