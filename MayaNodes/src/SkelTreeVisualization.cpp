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
MString	SkelTreeVisualization::drawDbClassification("drawdb/subscene/skelTreeVisualization");
MString	SkelTreeVisualization::drawRegistrantId("skelTreeVisualization");

MObject SkelTreeVisualization::mInSkelTreeData;
MObject SkelTreeVisualization::mTime;
MObject SkelTreeVisualization::nTimeOffset;

MObject SkelTreeVisualization::mNoiseTrunkVFO;
MObject SkelTreeVisualization::nNoiseTrunkValue;
MObject SkelTreeVisualization::nNoiseTrunkUFreq;
MObject SkelTreeVisualization::nNoiseTrunkUTimeFreq;
MObject SkelTreeVisualization::nNoiseTrunkTimeFreq;

MObject SkelTreeVisualization::mNoiseBranchVFO;
MObject SkelTreeVisualization::mNoiseLeafVFO;
MObject SkelTreeVisualization::mWindDirection;
MObject SkelTreeVisualization::mSegmentNum;
MObject SkelTreeVisualization::mNoiseGraphScale;

MObject SkelTreeVisualization::mDispChainAxisScale;

MObject SkelTreeVisualization::mDispEnableDeformedPoints;
MObject SkelTreeVisualization::mDispEnableDeformedMeshes;
MObject SkelTreeVisualization::mDispEnableChainBoxes;
MObject SkelTreeVisualization::mDispEnableChainLine;
MObject SkelTreeVisualization::mDispEnableAttachedPoint;
MObject SkelTreeVisualization::mDispEnableFoliages;
MObject SkelTreeVisualization::mDispEnableNoiseGraph;

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
	nTimeOffset = nAttr.create("timeOffset", "tof", MFnNumericData::kDouble, 0.0, &status);
	status = addAttribute(nTimeOffset);

	nNoiseTrunkValue = nAttr.create("noiseTrunkValue", "ntv", MFnNumericData::kDouble, 0.5, &status);
	nNoiseTrunkUFreq = nAttr.create("noiseTrunkUFreq", "nuq", MFnNumericData::kDouble, 0.5, &status);
	nNoiseTrunkUTimeFreq = nAttr.create("noiseTrunkUTimeFreq", "nut", MFnNumericData::kDouble, 0.2, &status);
	nNoiseTrunkTimeFreq = nAttr.create("noiseTrunkTimeFreq", "ntt", MFnNumericData::kDouble, 0.01, &status);
	
	mNoiseTrunkVFO = nAttr.create("noiseTrunkValueFreqOffset", "nto", 
		nNoiseTrunkValue, nNoiseTrunkUFreq, nNoiseTrunkUTimeFreq, nNoiseTrunkTimeFreq, &status);
	status = addAttribute(mNoiseTrunkVFO);

	mNoiseBranchVFO = nAttr.createPoint("noiseBranchValueFreqOffset", "nbv", &status);
	nAttr.setDefault(0.1, 5, 0, 0);
	status = addAttribute(mNoiseBranchVFO);

	mNoiseLeafVFO = nAttr.createPoint("noiseLeafValueFreqOffset","nlv", &status);
	nAttr.setDefault(1, 20, 0, 0);
	status = addAttribute(mNoiseLeafVFO);

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

	mDispEnableNoiseGraph = nAttr.create("dispEnableNoiseGraph", "eng", MFnNumericData::kBoolean, 1, &status);
	status = addAttribute(mDispEnableNoiseGraph);

	mSegmentNum = nAttr.create("segementNum", "snm", MFnNumericData::kInt, 5, &status);
	status = addAttribute(mSegmentNum);

	mNoiseGraphScale = nAttr.create("noiseGraphScale", "ngs", MFnNumericData::kDouble, 0.1, &status);
	status = addAttribute(mNoiseGraphScale);
	
	return status;
}

MBoundingBox SkelTreeVisualization::boundingBox() const
{
	MBoundingBox bbox;

	skelTree::RSkelTreeData treeData = getSkelTreeData();
	skelTree::CRBox stBox = treeData.boundBox;

	bbox.expand(MPoint(stBox.min.x, stBox.min.y, stBox.min.z));
	bbox.expand(MPoint(stBox.max.x, stBox.max.y, stBox.max.z));

	return bbox;
}

skelTree::RSkelTreeData SkelTreeVisualization::getSkelTreeData() const
{
	MObject thisNode = thisMObject();
	MPlug skelTreeDataPlug(thisNode, mInSkelTreeData);
	MPxData* pData = skelTreeDataPlug.asMDataHandle().asPluginData();
	assert(pData!=nullptr);
	return ((SkelTreeData*)pData)->skelTreeData;
}

skelTree::AniOpData SkelTreeVisualization::aniOpData()
{
	skelTree::AniOpData opData;

	MObject thisNode = thisMObject();

	MPlug timePlug(thisNode, mTime);
	opData.time = float(timePlug.asMTime().value());

	MPlug timeOffsetPlug(thisNode, nTimeOffset);
	opData.offsetTime(float(timeOffsetPlug.asDouble()));

	MPlug noiseTrunkPlug(thisNode, mNoiseTrunkVFO);
	opData.noiseTrunk[0] = float(noiseTrunkPlug.child(0).asDouble());
	opData.noiseTrunk[1] = float(noiseTrunkPlug.child(1).asDouble());
	opData.noiseTrunk[2] = float(noiseTrunkPlug.child(2).asDouble());
	opData.noiseTrunk[3] = float(noiseTrunkPlug.child(3).asDouble());

	MPlug noiseBranchPlug(thisNode, mNoiseBranchVFO);
	opData.noiseBranch[0] = float(noiseBranchPlug.child(0).asDouble());
	opData.noiseBranch[1] = float(noiseBranchPlug.child(1).asDouble());
	opData.noiseBranch[2] = float(noiseBranchPlug.child(2).asDouble());

	MPlug noiseFoliagePlug(thisNode, mNoiseLeafVFO);
	opData.noiseFoliage[0] = float(noiseFoliagePlug.child(0).asDouble());
	opData.noiseFoliage[1] = float(noiseFoliagePlug.child(1).asDouble());
	opData.noiseFoliage[2] = float(noiseFoliagePlug.child(2).asDouble());

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

	MPlug segNumPlug(thisNode, mSegmentNum);
	geoData.segmentNum = segNumPlug.asInt();

	MPlug noiseGraphScalePlug(thisNode, mNoiseGraphScale);
	geoData.noiseGraphScale = float(noiseGraphScalePlug.asDouble());

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
const MString SkelTreeVisualizationOverride::sChainBoxes = "skelTreeVisChainBoxes";
const MString SkelTreeVisualizationOverride::sChainLine = "skelTreeChainLine";
const MString SkelTreeVisualizationOverride::sAttachedPointName = "skelTreeAttachedPoint";
const MString SkelTreeVisualizationOverride::sDeformedMeshName = "skelTreeDeformedMesh";
const MString SkelTreeVisualizationOverride::sFoliageName = "skelTreeFoliages";
const MString SkelTreeVisualizationOverride::sNoiseGraph = "skelTreeNoiseGraph";

DispEnableMap SkelTreeVisualization::dispEnableData() const
{
	MObject thisNode = thisMObject();

	DispEnableMap visElementMap;

	bool updateFlag = false;

	MPlug enChainAxisPlug(thisNode, mDispEnableChainBoxes);
	visElementMap[VIS_ELEMENT_CHAIN_BOXES] = { &SkelTreeVisualizationOverride::sChainBoxes, enChainAxisPlug.asBool(), updateFlag };

	MPlug enChainLinePlug(thisNode, mDispEnableChainLine);
	visElementMap[VIS_ELEMENT_CHAIN_LINE] = { &SkelTreeVisualizationOverride::sChainLine, enChainLinePlug.asBool(), updateFlag };

	MPlug enDeformedPointsPlug(thisNode, mDispEnableDeformedPoints);
	visElementMap[VIS_ELEMENT_DEFORMED_POINTS] = { &SkelTreeVisualizationOverride::sDeformedPoints, enDeformedPointsPlug.asBool(), updateFlag };

	MPlug enDeformedMeshesPlug(thisNode, mDispEnableDeformedMeshes);
	visElementMap[VIS_ELEMENT_DEFORMED_TRIANGLES] = { &SkelTreeVisualizationOverride::sDeformedMeshName, enDeformedMeshesPlug.asBool(), updateFlag };

	MPlug enAttachedPointPlug(thisNode, mDispEnableAttachedPoint);
	visElementMap[VIS_ELEMENT_ATTACHED_POINT] = { &SkelTreeVisualizationOverride::sAttachedPointName, enAttachedPointPlug.asBool(), updateFlag };

	MPlug enFoliagePlug(thisNode, mDispEnableFoliages);
	visElementMap[VIS_ELEMENT_FOLIAGES] = { &SkelTreeVisualizationOverride::sFoliageName, enFoliagePlug.asBool(), updateFlag };

	MPlug enNoiseGraphPlug(thisNode, mDispEnableNoiseGraph);
	visElementMap[VIS_ELEMENT_NOISE_GRAPH] = { &SkelTreeVisualizationOverride::sNoiseGraph, enNoiseGraphPlug.asBool(), updateFlag };

	return visElementMap;
}

SkelTreeVisualizationOverride::SkelTreeVisualizationOverride(const MObject& obj)
	: MPxSubSceneOverride(obj)
	, mVisNode(nullptr)
{
	MStatus returnStatus;
	MFnDependencyNode dependNode(obj, &returnStatus);
	if (returnStatus != MStatus::kSuccess) return;
	MPxNode* userNode = dependNode.userNode(&returnStatus);
	if (returnStatus != MStatus::kSuccess) userNode = nullptr;
	mVisNode = dynamic_cast<SkelTreeVisualization*>(userNode);
	_geoUpdateFlag = true;
	_currentTime = 0.0f;
}

bool SkelTreeVisualizationOverride::hasUIDrawables() const
{
	return false;
}

bool SkelTreeVisualizationOverride::requiresUpdate(
	const MHWRender::MSubSceneContainer& container,
	const MHWRender::MFrameContext& frameContext) const
{
	// Nothing in the container, definitely need to update
	if (container.count() == 0)
	{
		return true;
	}

	skelTree::CRSkelTreeData treeData = mVisNode->getSkelTreeData();

	return treeData.isValid();
}

void SkelTreeVisualizationOverride::update(
	MHWRender::MSubSceneContainer& container,
	const MHWRender::MFrameContext& frameContext)
{
	pTreeData = &mVisNode->getSkelTreeData();

	if (_geoUpdateFlag) {
		buildGeometryStruct();
		_geoUpdateFlag = false;
	}

	updateGeometry();
	buildRenderItems(container);

}

void SkelTreeVisualizationOverride::buildGeometryStruct()
{
	skelTree.buildStruct(pTreeData);
	mVisNode->deformedMeshVertexIndices(triangleVtx);
}

void SkelTreeVisualizationOverride::updateGeometry()
{
	// prepare geometries
	aniOpData = mVisNode->aniOpData();
	popGeoData = mVisNode->popGeoData();
	visElementMap = mVisNode->dispEnableData();

	if (_currentTime != aniOpData.time) {
		skelTree.deformAndFoliages(pTreeData, aniOpData, visElementMap[VIS_ELEMENT_FOLIAGES].enable);
		for (auto& vis : visElementMap) {
			vis.second.update = true;
		}

		_currentTime = aniOpData.time;
	}

	if(visElementMap[VIS_ELEMENT_NOISE_GRAPH].enable)
		visElementMap[VIS_ELEMENT_NOISE_GRAPH].update = true;
}

void SkelTreeVisualizationOverride::buildRenderItems(MHWRender::MSubSceneContainer& container)
{
	DrawableBufferParam param;
	param.pTree = &skelTree;
	param.pTreeData = pTreeData;
	param.pPopGeoData = &popGeoData;
	param.pTriangleVtx = &triangleVtx;
	param.pAniOpData = &aniOpData;

	MBoundingBox bbox;
	bbox.expand(MPoint(pTreeData->boundBox.min.x, pTreeData->boundBox.min.y, pTreeData->boundBox.min.z));
	bbox.expand(MPoint(pTreeData->boundBox.max.x, pTreeData->boundBox.max.y, pTreeData->boundBox.max.z));

	RenderBufferManager& bufManager = RenderBufferManager::getInstance();

	// build render items
	for (auto renderItemData : visElementMap) {
		const MString& renderItemName = *renderItemData.second.name;
		MRenderItem* pItem = container.find(renderItemName);
		DrawItem* pBuf = bufManager.get(renderItemName);
		if (!pBuf)
			continue;

		if (renderItemData.second.enable) {
			if (!pItem || renderItemData.second.update) {
				if (!pItem)
				{
					pItem = pBuf->pRenderItem->create(renderItemName);
					container.add(pItem);
				}

				MVertexBufferArray vtxBufArray;
				MIndexBuffer* pIdxBuf = nullptr;

				pBuf->pDrawBuf->build(vtxBufArray, pIdxBuf, param);

				MStatus status = setGeometryForRenderItem(*pItem, vtxBufArray, *pIdxBuf, &bbox);

				vtxBufArray.clear();
				DELETE_POINTER(pIdxBuf);

				if (pBuf->pDrawBuf->hasInstance()) {
					MMatrixArray matArray;
					pBuf->pDrawBuf->instanceMatrices(matArray, param);
					setInstanceTransformArray(*pItem, matArray);
				}

				renderItemData.second.update = false;
			}
		} else {
			if (pItem) {
				container.remove(renderItemName);
				if (pBuf) {
					if (pBuf->pDrawBuf->hasInstance())
						removeAllInstances(*pItem);
				}
			}
		}
	}
}