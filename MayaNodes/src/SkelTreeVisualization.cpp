#include <SkelTreeVisualization.h>
#include <SkelTreeData.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MFnPluginData.h>

MTypeId SkelTreeVisualization::id(0x20220003);
MString	SkelTreeVisualization::drawDbClassification("drawdb/geometry/skelTreeVisualization");
MString	SkelTreeVisualization::drawRegistrantId("skelTreeVisualization");

MObject SkelTreeVisualization::mInSkelTreeData;

void* SkelTreeVisualization::creator()
{
	return new SkelTreeVisualization();
}

MStatus SkelTreeVisualization::initialize()
{
	MStatus status;

	MFnTypedAttribute		tAttr;
	MFnPluginData			fnDataCreator;
	MTypeId					skelTreeDataId(SkelTreeData::id);
	mInSkelTreeData = tAttr.create("inSkelTreeData", "ist", SkelTreeData::id, fnDataCreator.create(skelTreeDataId));
	tAttr.setStorable(false);

	status = addAttribute(mInSkelTreeData);

	return status;
}

MBoundingBox SkelTreeVisualization::boundingBox() const
{
	MBoundingBox bbox;

	skelTree::CRSkelTreeData treeData = getSkelTreeData();
	skelTree::CRBox stBox = treeData.boundBox;

	bbox.expand(MPoint(stBox.min.x, stBox.min.y, stBox.min.z));
	bbox.expand(MPoint(stBox.max.x, stBox.max.y, stBox.max.z));

	return bbox;
}

skelTree::RSkelTreeData SkelTreeVisualization::getSkelTreeData() const
{
	MObject thisNode = thisMObject();
	MPlug skelTreeDataPlug(thisNode, mInSkelTreeData);
	return ((SkelTreeData*)skelTreeDataPlug.asMDataHandle().asPluginData())->skelTreeData;
}

const MString SkelTreeVisualizationOverride::sDeformedPoints = "skelTreeVisDeformedPoints";

SkelTreeVisualizationOverride::SkelTreeVisualizationOverride(const MObject& obj)
	: MPxGeometryOverride(obj)
	, mVisNode(nullptr)
{
	MStatus returnStatus;
	MFnDependencyNode dependNode(obj, &returnStatus);
	if (returnStatus != MStatus::kSuccess) return;
	MPxNode* footPrintNode = dependNode.userNode(&returnStatus);
	if (returnStatus != MStatus::kSuccess) footPrintNode = nullptr;
	mVisNode = dynamic_cast<SkelTreeVisualization*>(footPrintNode);
}

void SkelTreeVisualizationOverride::updateDG()
{
	pTreeData = &mVisNode->getSkelTreeData();
	renderBufferManagerBuild(bufManager, sDeformedPoints);
}

void SkelTreeVisualizationOverride::updateRenderItems(const MDagPath& path, MRenderItemList& list)
{
	MHWRender::MRenderItem* vertexItem = NULL;
	int index = list.indexOf(sDeformedPoints);
	if (index < 0)
	{
		vertexItem = bufManager.get(sDeformedPoints).pRenderItem->create(sDeformedPoints);
		list.append(vertexItem);
	}
}

void SkelTreeVisualizationOverride::populateGeometry(const MGeometryRequirements& requirements, const MRenderItemList& renderItems, MGeometry& data)
{
	const MVertexBufferDescriptorList& vertexBufferDescriptorList = requirements.vertexRequirements();
	const int numberOfVertexRequirments = vertexBufferDescriptorList.length();

	// vertex buffer
	MVertexBufferDescriptor vertexBufferDescriptor;
	for (int requirmentNumber = 0; requirmentNumber < numberOfVertexRequirments; ++requirmentNumber)
	{
		if (!vertexBufferDescriptorList.getDescriptor(requirmentNumber, vertexBufferDescriptor))
			continue;

		bufManager.get(sDeformedPoints).pPopulate->populateGeometryVertex(pTreeData, data, vertexBufferDescriptor);
	}

	// index buffer
	for (int i = 0; i < renderItems.length(); ++i)
	{
		const MRenderItem* item = renderItems.itemAt(i);

		if (!item)
			continue;

		bufManager.get(sDeformedPoints).pPopulate->populateGeometryIndex(pTreeData, data, item, sDeformedPoints);
	}
}
