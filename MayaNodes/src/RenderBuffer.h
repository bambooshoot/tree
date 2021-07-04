#pragma once

#include <string>
#include <map>
#include <maya/MString.h>
#include <RenderItemBase.h>

#include <PopulateGeometryChains.h>
#include <PopulateGeometryDeformedPoints.h>
#include <PopulateGeometryChainLine.h>
#include <PopulateGeometryMesh.h>

#include <RenderItemWireframe.h>
#include <RenderItemCPVPoint.h>
#include <RenderItemTriangles.h>

struct RenderBuffer
{
	PopulateGeometryBase*	pPopulate;
	RenderItemBase*			pRenderItem;
};

#define VIS_ELEMENT_CHAIN_LINE				0
#define VIS_ELEMENT_CHAIN_AXIS				1
#define VIS_ELEMENT_DEFORMED_POINTS			2
#define VIS_ELEMENT_DEFORMED_TRIANGLES		3

struct DispElementEnableData
{
	const MString* name;
	bool enable;
};

using DispEnableMap = std::map<uint, DispElementEnableData>;

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
		case MGeometry::kNormal:
		{
			MVertexBuffer* cpvBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* nml = (float*)cpvBuffer->acquire(vertexSize, true);
			for (auto& buf : bufMap) {
				buf.second.pPopulate->populateGeometryNormal(data, vertexBufferDescriptor, nml);
			}
			cpvBuffer->commit(nml);
		}
		break;
		default:
			// do nothing for stuff we don't understand
			break;
		}
	}
	void registerBuffer(const MString* renderItemName, RenderBuffer& buffer)
	{
		bufMap.insert(std::pair<std::string, RenderBuffer>(std::string(renderItemName->asChar()), buffer));
	}
	void perpare(skelTree::SkelTreeDataP pTreeData, skelTree::SkelTreeP pTree, PopulateGeometryData* pPopGeoData)
	{
		uint vertexOffsetId = 0;
		for (auto& buf : bufMap) {
			buf.second.pPopulate->prepare(pTreeData, pTree, pPopGeoData, vertexOffsetId);
			vertexOffsetId += buf.second.pPopulate->vertexSize();
		}
	}
	RenderBuffer* get(const MString& renderItemName)
	{
		std::string keyStr = std::string(renderItemName.asChar());
		if (bufMap.find(keyStr) != bufMap.end())
			return &bufMap[keyStr];
		return nullptr;
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

inline void renderBufferManagerBuild(
	RenderBufferManager &	manager,
	skelTree::SkelTreeDataP pTreeData,
	skelTree::SkelTreeP		pTree,
	PopulateGeometryData*	pPopGeoData,
	DispEnableMap&			dispEnableMap)
{
	manager.clear();

	RenderBuffer buf;

	// draw space
	if (dispEnableMap[VIS_ELEMENT_CHAIN_AXIS].enable) {
		buf.pRenderItem = new RenderItemWireframe();
		buf.pPopulate = new PopulateGeometryChains();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_CHAIN_AXIS].name, buf);
	}

	// draw deformed points
	if (dispEnableMap[VIS_ELEMENT_DEFORMED_POINTS].enable) {
		buf.pRenderItem = new RenderItemCPVPoint();
		buf.pPopulate = new PopulateGeometryDeformedPoints();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_DEFORMED_POINTS].name, buf);
	}

	// draw chain lines
	if (dispEnableMap[VIS_ELEMENT_CHAIN_LINE].enable) {
		buf.pRenderItem = new RenderItemWireframe();
		buf.pPopulate = new PopulateGeometryChainLine();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_CHAIN_LINE].name, buf);
	}

	// draw deformed triangles
	if (dispEnableMap[VIS_ELEMENT_DEFORMED_TRIANGLES].enable) {
		buf.pRenderItem = new RenderItemTriangles();
		buf.pPopulate = new PopulateGeometryMesh();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_DEFORMED_TRIANGLES].name, buf);
	}

	manager.perpare(pTreeData, pTree, pPopGeoData);
}