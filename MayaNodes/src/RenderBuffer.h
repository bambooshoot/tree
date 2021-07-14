#pragma once

#include <string>
#include <map>
#include <maya/MString.h>

#include <DrawableBufferAttachedPoint.h>
#include <DrawableBufferChainBoxes.h>
#include <DrawableBufferChainLine.h>
#include <DrawableBufferDeformedPoints.h>
#include <DrawableBufferFoliages.h>
#include <DrawableBufferMesh.h>

#include <RenderItemBase.h>
#include <RenderItemWireframe.h>
#include <RenderItemCPVPoint.h>
#include <RenderItemTriangles.h>
#include <RenderItemCPVTriangles.h>

#define VIS_ELEMENT_CHAIN_LINE				0
#define VIS_ELEMENT_ATTACHED_POINT			1
#define VIS_ELEMENT_DEFORMED_POINTS			2
#define VIS_ELEMENT_DEFORMED_TRIANGLES		3
#define VIS_ELEMENT_CHAIN_BOXES				4
#define VIS_ELEMENT_FOLIAGES				5

struct DispElementEnableData
{
	const MString* name;
	bool enable;
};

struct DrawItem
{
	RenderItemBase* pRenderItem;
	DrawableBufferBase* pDrawBuf;
	DrawItem() : pRenderItem(nullptr), pDrawBuf(nullptr) {};
	~DrawItem()
	{
		DELETE_POINTER(pRenderItem);
		DELETE_POINTER(pDrawBuf);
	}
};

using DrawItemPtr = std::shared_ptr<DrawItem>;

using DispEnableMap = std::map<uint, DispElementEnableData>;

class RenderBufferManager
{
public:
	~RenderBufferManager()
	{
		clear();
	}
	void registerBuffer(const MString renderItemName, RenderItemBase* pRenderItem, DrawableBufferBase* pDrawBuf)
	{
		DrawItem ditm;
		const std::string key(renderItemName.asChar());
		bufMap.insert( std::make_pair(key, ditm) );
		DrawItem& rd = bufMap[key];
		rd.pDrawBuf = pDrawBuf;
		rd.pRenderItem = pRenderItem;
	}
	DrawItem* get(const MString renderItemName)
	{
		const std::string key(renderItemName.asChar());
		if (bufMap.find(key) != bufMap.end())
			return &bufMap[key];

		return nullptr;
	}
	void clear()
	{
		//for (auto& iter : bufMap)
		//	iter.second.clear();

		bufMap.clear();
	}
	void setParam(DrawableBufferParam& param)
	{
		for (auto& buf : bufMap) {
			param.name = buf.first;
			buf.second.pDrawBuf->setParam(param);
		}
	}

private:
	std::map<std::string, DrawItem> bufMap;
};
