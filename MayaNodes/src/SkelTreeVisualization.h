#pragma once

#include <maya/MPxLocatorNode.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxSubSceneOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MDataBlock.h>

#include <SkelChainOpBase.h>
#include <SkelTree.h>
#include <RenderBuffer.h>

class SkelTreeVisualization : public MPxLocatorNode
{
public:
	SkelTreeVisualization() {};
	~SkelTreeVisualization() override {};

	//bool setInternalValue(const MPlug&, const MDataHandle&) override;

	static  void*   creator();
	static  MStatus initialize();

	bool            isTransparent() const override { return true; }
	bool            drawLast() const override { return true; }
	bool            isBounded() const override { return true; }
	MBoundingBox    boundingBox() const override;

	skelTree::RSkelTreeData getSkelTreeData() const;
	skelTree::AniOpData aniOpData();
	PopulateGeometryData popGeoData() const;
	DispEnableMap dispEnableData() const;
	void deformedMeshVertexIndices(std::vector<MIntArray>& idxArray) const;

	static MTypeId id;
	static MString drawDbClassification;
	static MString drawRegistrantId;

	static MObject mInSkelTreeData;
	static MObject mTime;
	static MObject nTimeOffset;
	static MObject mNoiseTrunkVFO;
	static MObject nNoiseTrunkValue;
	static MObject nNoiseTrunkUFreq;
	static MObject nNoiseTrunkUTimeFreq;
	static MObject nNoiseTrunkTimeFreq;
	static MObject mNoiseBranchVFO;
	static MObject mNoiseLeafVFO;
	static MObject mWindDirection;
	static MObject mSegmentNum;
	static MObject mNoiseGraphScale;

	static MObject mDispChainAxisScale;
	static MObject mDispEnableDeformedPoints;
	static MObject mDispEnableChainBoxes;
	static MObject mDispEnableChainLine;
	static MObject mDispEnableDeformedMeshes;
	static MObject mDispEnableAttachedPoint;
	static MObject mDispEnableFoliages;
	static MObject mDispEnableNoiseGraph;
};

using namespace MHWRender;


class SkelTreeVisualizationOverride : public MPxSubSceneOverride
{
public:
	static MPxSubSceneOverride* Creator(const MObject& obj) { return new SkelTreeVisualizationOverride(obj); }
	~SkelTreeVisualizationOverride() override {};

	// Support configuration functions :
	DrawAPI supportedDrawAPIs() const override { return  MHWRender::kAllDevices; }
	bool hasUIDrawables() const override;

	bool requiresUpdate(
		const MHWRender::MSubSceneContainer& container,
		const MHWRender::MFrameContext& frameContext) const override;

	void update(
		MHWRender::MSubSceneContainer& container,
		const MHWRender::MFrameContext& frameContext) override;

	static const MString sDeformedPoints;
	static const MString sChainBoxes;
	static const MString sChainLine;
	static const MString sDeformedMeshName;
	static const MString sAttachedPointName;
	static const MString sFoliageName;
	static const MString sNoiseGraph;

private:
	SkelTreeVisualizationOverride(const MObject& obj);

	void updateGeometry();
	void buildGeometryStruct();
	void buildRenderItems(MHWRender::MSubSceneContainer& container);

	bool						_geoUpdateFlag;
	float						_currentTime;

	SkelTreeVisualization*		mVisNode;
	skelTree::SkelTreeDataP		pTreeData;
	skelTree::SkelTree			skelTree;

	PopulateGeometryData		popGeoData;
	DispEnableMap				visElementMap;
	std::vector<MIntArray>		triangleVtx;
};