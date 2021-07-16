#pragma once

#include <SkelTreeData.h>
#include <SkelChainOpBase.h>
#include <SkelTreeByte.h>

NS_BEGIN

class File
{
public:
	void write(SkelTreeData& treeData, AniOpData& opData, CCharP fileName);
	void read(SkelTreeData& treeData, AniOpData& opData);
};

NS_END