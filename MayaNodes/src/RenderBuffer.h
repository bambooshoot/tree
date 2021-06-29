#pragma once

#include <string>
#include <map>
#include <maya/MString.h>
#include <RenderItemBase.h>

#include <PopulateGeometryChains.h>
#include <PopulateGeometryDeformedPoints.h>

#include <RenderItemWireframe.h>
#include <RenderItemCPVPoint.h>

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
	void createVertexBuffer(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor)
	{
		uint vertexSize = 0;
		for (auto& buf : bufMap) {
			vertexSize += buf.second.pPopulate->vertexSize();
		}

		switch (vertexBufferDescriptor.semantic())
		{
		case MGeometry::kPosition:
		{
			MVertexBuffer* verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* vert = (float*)verticesBuffer->acquire(vertexSize, true);
			for (auto& buf : bufMap) {
				buf.second.pPopulate->populateGeometryPosition(data, vertexBufferDescriptor, vert);
			}
			verticesBuffer->commit(vert);
		}
		break;
		case MGeometry::kColor:
		{
			MVertexBuffer* cpvBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* clr = (float*)cpvBuffer->acquire(vertexSize, true);
			for (auto& buf : bufMap) {
				buf.second.pPopulate->populateGeometryColor(data, vertexBufferDescriptor, clr);
			}
			cpvBuffer->commit(clr);
		}
		break;
		default:
			// do nothing for stuff we don't understand
			break;
		}
	}
	void registerBuffer(const MString& renderItemName, RenderBuffer& buffer)
	{
		bufMap.insert(std::pair<std::string, RenderBuffer>(std::string(renderItemName.asChar()), buffer));
	}
	void perpare(skelTree::SkelTreeDataP pTreeData)
	{
		uint indexOffsetId = 0;
		uint vertexOffsetId = 0;
		for (auto& buf : bufMap) {
			buf.second.pPopulate->prepare(pTreeData, vertexOffsetId, indexOffsetId);
			indexOffsetId += buf.second.pPopulate->indexSize();
			vertexOffsetId += buf.second.pPopulate->vertexSize();
		}
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

inline void renderBufferManagerBuild(RenderBufferManager & manager,
	skelTree::SkelTreeDataP pTreeData,
	const MString & deformedPointsName,
	const MString & spaceName)
{
	manager.clear();

	// draw space
	RenderBuffer buf;
	buf.pRenderItem = new RenderItemWireframe();
	buf.pPopulate = new PopulateGeometryChains();
	manager.registerBuffer(spaceName, buf);

	// draw deformed points
	buf.pRenderItem = new RenderItemCPVPoint();
	buf.pPopulate = new PopulateGeometryDeformedPoints();
	manager.registerBuffer(deformedPointsName, buf);

	manager.perpare(pTreeData);
}