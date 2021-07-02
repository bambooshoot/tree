#include <SkelTreeVisualization.h>
#include <SkelTreeNodeData.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>

#include <maya/MFnUnitAttribute.h>

#include <maya/MFnPluginData.h>

MTypeId SkelTreeVisualization::id(0x20220003);
MString	SkelTreeVisualization::drawDbClassification("drawdb/geometry/skelTreeVisualization");
MString	SkelTreeVisualization::drawRegistrantId("skelTreeVisualization");

MObject SkelTreeVisualization::mInSkelTreeData;
MObject SkelTreeVisualization::mTime;
MObject SkelTreeVisualization::mNoiseValue;

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
	status = addAttribute(mInSkelTreeData);

	MFnUnitAttribute		uAttr;
	mTime = uAttr.create("time", "tim", MFnUnitAttribute::kTime, 1);
	status = addAttribute(mTime);

	MFnNumericAttribute nAttr;
	mNoiseValue = nAttr.create("noiseValue","noi", MFnNumericData::kDouble, 1.0, &status);
	status = addAttribute(mNoiseValue);
	
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

float SkelTreeVisualization::time() const
{
	MObject thisNode = thisMObject();
	MPlug timePlug(thisNode, mTime);
	return float(timePlug.asMTime().value());
}

float SkelTreeVisualization::noiseValue() const
{
	MObject thisNode = thisMObject();
	MPlug timePlug(thisNode, mNoiseValue);
	return float(timePlug.asDouble());
}

const MString SkelTreeVisualizationOverride::sDeformedPoints = "skelTreeVisDeformedPoints";
const MString SkelTreeVisualizationOverride::sSpace = "skelTreeVisSpace";

const MString* SkelTreeVisualizationOverride::renderItemNames[2] = { &sDeformedPoints, &sSpace };

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

bool SkelTreeVisualizationOverride::requiresGeometryUpdate() const
{
	return true;
}

void SkelTreeVisualizationOverride::updateDG()
{
	float curTime = mVisNode->time();
	float noiseValue = mVisNode->noiseValue();
	pTreeData = &mVisNode->getSkelTreeData();
	skelTree.reset(pTreeData);
	skelTree.buildChains();
	skelTree.deform(curTime, noiseValue);

	renderBufferManagerBuild(bufManager, pTreeData, &skelTree, sDeformedPoints, sSpace);
}

void SkelTreeVisualizationOverride::updateRenderItems(const MDagPath& path, MRenderItemList& list)
{
	MHWRender::MRenderItem* vertexItem = NULL;

	for (auto renderItemName : renderItemNames) {
		int index = list.indexOf(*renderItemName);
		if (index < 0)
		{
			vertexItem = bufManager.get(*renderItemName).pRenderItem->create(*renderItemName);
			list.append(vertexItem);
		}
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

		bufManager.createVertexBuffer(data, vertexBufferDescriptor);
	}

	// index buffer
	for (int i = 0; i < renderItems.length(); ++i)
	{
		const MRenderItem* item = renderItems.itemAt(i);

		if (!item)
			continue;

		const MString& renderItemName = item->name();
		bufManager.get(renderItemName).pPopulate->populateGeometryIndex(data, item, renderItemName);
	}
}
