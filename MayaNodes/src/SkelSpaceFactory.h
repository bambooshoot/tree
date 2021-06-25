#pragma once

#include <SkelSpace.h>
#include <SkelAttachedPoint.h>
#include <SkelRootFrame.h>
#include <SkelFrame.h>
#include <SkelChainData.h>

NS_BEGIN

using CreateFunc = SpaceP(*)(CVoidP, CChainDataP pChainData);

class SpaceFactory
{
	using FuncMap = std::map<SpaceTypeId, CreateFunc>;
	using FuncMapPair = std::pair<SpaceTypeId, CreateFunc>;

public:
	SpaceFactory()
	{
		registerType(AttachedPoint::typeId, AttachedPoint::create);
		registerType(RootFrame::typeId, RootFrame::create);
		registerType(Frame::typeId, Frame::create);
	}
	SpaceP create(CSpaceTypeId typeId, CVoidP data, CChainDataP pChainData)
	{
		return (*createFuncMap[typeId])(data, pChainData);
	}
	void registerType(CSpaceTypeId typeId, CreateFunc pFunc)
	{
		createFuncMap.insert(FuncMapPair(typeId, pFunc));
	}

private:
	FuncMap createFuncMap;
};

NS_END