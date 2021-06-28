#pragma once

#include <string>
#include <map>
#include <maya/MString.h>
#include <RenderItemBase.h>

#include <PopulateGeometryChains.h>
#include <PopulateGeometryDeformedPoints.h>

#include <RenderItemWireframe.h>
#include <RenderItemCVPPoint.h>

struct RenderBuffer
{
	PopulateGeometryBase*	pPopulate;
	RenderItemBase*			pRenderItem;
};

class RenderBufferManager
{
public:
	~RenderBufferManager()
	{
		clear();
	}
	void registerBuffer(const MString& renderItemName, RenderBuffer& buffer)
	{
		bufMap.insert(std::pair<std::string, RenderBuffer>(std::string(renderItemName.asChar()), buffer));
	}
	RenderBuffer& get(const MString& renderItemName)
	{
		return bufMap[std::string(renderItemName.asChar())];
	}
	void clear()
	{
		for (auto& buf : bufMap) {
			delete buf.second.pPopulate;
			delete buf.second.pRenderItem;
		}

		bufMap.clear();
	}

private:
	std::map<std::string, RenderBuffer> bufMap;
};

inline void renderBufferManagerBuild(RenderBufferManager & manager,const MString & deformedPointsName)
{
	manager.clear();

	RenderBuffer bufDeformedPoints;
	bufDeformedPoints.pRenderItem = new RenderItemWireframe();
	bufDeformedPoints.pPopulate = new PopulateGeometryChains();
	manager.registerBuffer(deformedPointsName, bufDeformedPoints);
}