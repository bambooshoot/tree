#pragma once

#include <string>
#include <map>
#include <maya/MString.h>
#include <RenderItemBase.h>

#include <PopulateGeometryAttachedPoint.h>
#include <PopulateGeometryDeformedPoints.h>
#include <PopulateGeometryChainLine.h>
#include <PopulateGeometryMesh.h>
#include <PopulateGeometryChainBoxes.h>
#include <PopulateGeometryFoliages.h>

#include <RenderItemWireframe.h>
#include <RenderItemCPVPoint.h>
#include <RenderItemTriangles.h>
#include <RenderItemCPVTriangles.h>

struct RenderBuffer
{
	PopulateGeometryBase*	pPopulate;
	RenderItemBase*			pRenderItem;
};

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
		RenderBuffer* pBuf = get(vertexBufferDescriptor.name());
		if (pBuf == nullptr)
			return;

		uint vertexSize = pBuf->pPopulate->vertexSize();

		switch (vertexBufferDescriptor.semantic())
		{
		case MGeometry::kPosition:
		{
			MVertexBuffer* verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* vert = (float*)verticesBuffer->acquire(vertexSize, true);
			pBuf->pPopulate->populateGeometryPosition(data, vertexBufferDescriptor, vert);
			verticesBuffer->commit(vert);
		}
		break;
		case MGeometry::kColor:
		{
			MVertexBuffer* cpvBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* clr = (float*)cpvBuffer->acquire(vertexSize, true);
			pBuf->pPopulate->populateGeometryColor(data, vertexBufferDescriptor, clr);
			cpvBuffer->commit(clr);
		}
		break;
		case MGeometry::kNormal:
		{
			MVertexBuffer* nmlBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			float* nml = (float*)nmlBuffer->acquire(vertexSize, true);
			pBuf->pPopulate->populateGeometryNormal(data, vertexBufferDescriptor, nml);
			nmlBuffer->commit(nml);
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
		for (auto& buf : bufMap) {
			buf.second.pPopulate->prepare(pTreeData, pTree, pPopGeoData);
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

	// draw chain boxes
	if (dispEnableMap[VIS_ELEMENT_CHAIN_BOXES].enable) {
		buf.pRenderItem = new RenderItemCPVTriangles();
		buf.pPopulate = new PopulateGeometryChainBoxes();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_CHAIN_BOXES].name, buf);
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

	// draw attached point
	if (dispEnableMap[VIS_ELEMENT_ATTACHED_POINT].enable) {
		buf.pRenderItem = new RenderItemWireframe();
		buf.pPopulate = new PopulateGeometryAttachedPoint();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_ATTACHED_POINT].name, buf);
	}

	// draw foliages
	if (dispEnableMap[VIS_ELEMENT_FOLIAGES].enable) {
		buf.pRenderItem = new RenderItemTriangles();
		buf.pPopulate = new PopulateGeometryFoliages();
		manager.registerBuffer(dispEnableMap[VIS_ELEMENT_FOLIAGES].name, buf);
	}

	manager.perpare(pTreeData, pTree, pPopGeoData);
}