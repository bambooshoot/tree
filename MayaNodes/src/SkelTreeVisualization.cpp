#include <SkelTreeVisualization.h>
#include <SkelTreeNodeData.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MFnMesh.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MGlobal.h>

#include <maya/MFnUnitAttribute.h>

#include <maya/MFnPluginData.h>

#include <SkelTreeCreatorNode.h>

MTypeId SkelTreeVisualization::id(0x20220003);
MString	SkelTreeVisualization::drawDbClassification("drawdb/geometry/skelTreeVisualization");
MString	SkelTreeVisualization::drawRegistrantId("skelTreeVisualization");

MObject SkelTreeVisualization::mInSkelTreeData;
MObject SkelTreeVisualization::mTime;
MObject SkelTreeVisualization::mNoiseValue;
MObject SkelTreeVisualization::mNoiseFreqU;
MObject SkelTreeVisualization::mNoiseFreqChain;
MObject SkelTreeVisualization::mNoiseOffset;
MObject SkelTreeVisualization::mWindDirection;

MObject SkelTreeVisualization::mDispChainAxisScale;

MObject SkelTreeVisualization::mDispEnableDeformedPoints;
MObject SkelTreeVisualization::mDispEnableDeformedMeshes;
MObject SkelTreeVisualization::mDispEnableChainBoxes;
MObject SkelTreeVisualization::mDispEnableChainLine;
MObject SkelTreeVisualization::mDispEnableAttachedPoint;
MObject SkelTreeVisualization::mDispEnableFoliages;

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
	mNoiseFreqU = nAttr.create("noiseFreqU", "nfu", MFnNumericData::kDouble, 1.0, &status);
	status = addAttribute(mNoiseFreqU);

	mNoiseFreqChain = nAttr.create("noiseFreqChain", "nfc", MFnNumericData::kDouble, 1.0, &status);
	status = addAttribute(mNoiseFreqChain);

	mNoiseValue = nAttr.create("noiseValue","noi", MFnNumericData::kDouble, 0.1, &status);
	status = addAttribute(mNoiseValue);

	mNoiseOffset = nAttr.create("noiseOffset", "nof", MFnNumericData::kDouble, 0.5, &status);
	status = addAttribute(mNoiseOffset);

	mWindDirection = nAttr.createPoint("windDirection", "wdn", &status);
	nAttr.setDefault(0, 0, 1, 0);
	status = addAttribute(mWindDirection);

	mDispChainAxisScale = nAttr.create("dispChainScale", "dcs", MFnNumericData::kDouble, 0.1, &status);
	status = addAttribute(mDispChainAxisScale);

	mDispEnableDeformedPoints = nAttr.create("dispEnableDeformedPoints", "edp", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableDeformedPoints);

	mDispEnableDeformedMeshes = nAttr.create("dispEnableDeformedMeshes", "edm", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableDeformedMeshes);

	mDispEnableChainBoxes = nAttr.create("dispEnableChainBoxes", "ecb", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableChainBoxes);

	mDispEnableChainLine = nAttr.create("dispEnableChainLine", "ecl", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableChainLine);

	mDispEnableAttachedPoint = nAttr.create("dispEnableAttachedPoint", "eap", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableAttachedPoint);

	mDispEnableFoliages = nAttr.create("dispEnableFoliages", "efg", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableFoliages);
	
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

skelTree::ChainOpData SkelTreeVisualization::chainOpData()
{
	skelTree::ChainOpData opData;

	MObject thisNode = thisMObject();

	MPlug timePlug(thisNode, mTime);
	opData.time = float(timePlug.asMTime().value());

	MPlug noisePlug(thisNode, mNoiseValue);
	opData.scale = float(noisePlug.asDouble());

	MPlug offsetPlug(thisNode, mNoiseOffset);
	opData.offset = float(offsetPlug.asDouble());

	MPlug noiseFreqUPlug(thisNode, mNoiseFreqU);
	opData.freqU = float(noiseFreqUPlug.asDouble());

	MPlug noiseFreqChainPlug(thisNode, mNoiseFreqChain);
	opData.freqChain = float(noiseFreqChainPlug.asDouble());

	MPlug windDirPlug(thisNode, mWindDirection);
	opData.windDirection.x = float(windDirPlug.child(0).asDouble());
	opData.windDirection.y = float(windDirPlug.child(1).asDouble());
	opData.windDirection.z = float(windDirPlug.child(2).asDouble());

	return opData;
}

PopulateGeometryData SkelTreeVisualization::popGeoData() const
{
	PopulateGeometryData geoData;

	MObject thisNode = thisMObject();

	MPlug chainAxisScalePlug(thisNode, mDispChainAxisScale);
	geoData.chainAxisScale = float(chainAxisScalePlug.asDouble());

	deformedMeshVertexIndices(geoData.triangleVtx);

	return geoData;
}

void SkelTreeVisualization::deformedMeshVertexIndices(std::vector<MIntArray>& idxArray) const
{
	MObject thisNode = thisMObject();
	MPlug inTreeDataPlug(thisNode, mInSkelTreeData);

	MPlugArray plugArray;

	if (inTreeDataPlug.connectedTo(plugArray, 1, 0) == 0)
		return;

	MFnDependencyNode treeNode(plugArray[0].node());
	MPlug inMeshesPlug = treeNode.findPlug(SkelTreeCreator::mInMeshes);

	idxArray.resize(inMeshesPlug.numElements());
	for (uint i = 0; i < inMeshesPlug.numElements(); ++i) {
		MPlug curMeshPlug = inMeshesPlug.elementByPhysicalIndex(i);
		MFnMesh fnMesh(curMeshPlug.child(SkelTreeCreator::mInMesh).asMDataHandle().asMesh());
		MIntArray countArray;
		fnMesh.getTriangles(countArray, idxArray[i]);
	}
}

const MString SkelTreeVisualizationOverride::sDeformedPoints = "skelTreeVisDeformedPoints";
const MString SkelTreeVisualizationOverride::sSpace = "skelTreeVisSpace";
const MString SkelTreeVisualizationOverride::sJointName = "skelTreeJoint";
const MString SkelTreeVisualizationOverride::sAttachedPointName = "skelTreeAttachedPoint";
const MString SkelTreeVisualizationOverride::sMeshName = "skelTreeMesh";
const MString SkelTreeVisualizationOverride::sFoliageName = "skelTreeFoliages";

DispEnableMap SkelTreeVisualization::dispEnableData() const
{
	MObject thisNode = thisMObject();

	DispEnableMap visElementMap;

	MPlug enChainAxisPlug(thisNode, mDispEnableChainBoxes);
	visElementMap[VIS_ELEMENT_CHAIN_BOXES] = { &SkelTreeVisualizationOverride::sSpace, enChainAxisPlug.asBool() };

	MPlug enChainLinePlug(thisNode, mDispEnableChainLine);
	visElementMap[VIS_ELEMENT_CHAIN_LINE] = { &SkelTreeVisualizationOverride::sJointName, enChainLinePlug.asBool() };

	MPlug enDeformedPointsPlug(thisNode, mDispEnableDeformedPoints);
	visElementMap[VIS_ELEMENT_DEFORMED_POINTS] = { &SkelTreeVisualizationOverride::sDeformedPoints, enDeformedPointsPlug.asBool() };

	MPlug enDeformedMeshesPlug(thisNode, mDispEnableDeformedMeshes);
	visElementMap[VIS_ELEMENT_DEFORMED_TRIANGLES] = { &SkelTreeVisualizationOverride::sMeshName, enDeformedMeshesPlug.asBool() };

	MPlug enAttachedPointPlug(thisNode, mDispEnableAttachedPoint);
	visElementMap[VIS_ELEMENT_ATTACHED_POINT] = { &SkelTreeVisualizationOverride::sMeshName, enAttachedPointPlug.asBool() };

	MPlug enFoliagePlug(thisNode, mDispEnableFoliages);
	visElementMap[VIS_ELEMENT_FOLIAGES] = { &SkelTreeVisualizationOverride::sMeshName, enFoliagePlug.asBool() };

	return visElementMap;
}

SkelTreeVisualizationOverride::SkelTreeVisualizationOverride(const MObject& obj)
	: MPxGeometryOverride(obj)
	, mVisNode(nullptr)
{
	MStatus returnStatus;
	MFnDependencyNode dependNode(obj, &returnStatus);
	if (returnStatus != MStatus::kSuccess) return;
	MPxNode* userNode = dependNode.userNode(&returnStatus);
	if (returnStatus != MStatus::kSuccess) userNode = nullptr;
	mVisNode = dynamic_cast<SkelTreeVisualization*>(userNode);
}

bool SkelTreeVisualizationOverride::requiresGeometryUpdate() const
{
	return true;
}

void SkelTreeVisualizationOverride::updateDG()
{
	skelTree::ChainOpData opData = mVisNode->chainOpData();
	popGeoData = mVisNode->popGeoData();
	pTreeData = &mVisNode->getSkelTreeData();
	visElementMap = mVisNode->dispEnableData();

	skelTree.buildDeform(pTreeData, opData);

	renderBufferManagerBuild(bufManager, pTreeData, &skelTree, &popGeoData, visElementMap);
}

void SkelTreeVisualizationOverride::updateRenderItems(const MDagPath& path, MRenderItemList& list)
{
	MHWRender::MRenderItem* vertexItem = NULL;

	for (auto renderItemData : visElementMap) {
		const MString& renderItemName = *renderItemData.second.name;
		int index = list.indexOf(renderItemName);
		if (index < 0)
		{
			RenderBuffer* pBuf = bufManager.get(renderItemName);
			if (pBuf) {
				vertexItem = pBuf->pRenderItem->create(renderItemName);
				list.append(vertexItem);
			}
		}
		else if (renderItemData.second.enable == false){
			list.removeAt(index);
		}
	}
}

void SkelTreeVisualizationOverride::populateGeometry(
	const MGeometryRequirements& requirements, const MRenderItemList& renderItems, MGeometry& data)
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
		RenderBuffer* pBuf = bufManager.get(renderItemName);

		if (pBuf) {
			pBuf->pPopulate->populateGeometryIndex(data, item, renderItemName);
		}
	}
}
