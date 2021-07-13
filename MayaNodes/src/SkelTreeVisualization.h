#pragma once

#include <maya/MPxLocatorNode.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxGeometryOverride.h>
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
	static MObject mNoiseTrunkVFO;
	static MObject mNoiseBranchVFO;
	static MObject mNoiseLeafVFO;
	static MObject mWindDirection;

	static MObject mDispChainAxisScale;
	static MObject mDispEnableDeformedPoints;
	static MObject mDispEnableChainBoxes;
	static MObject mDispEnableChainLine;
	static MObject mDispEnableDeformedMeshes;
	static MObject mDispEnableAttachedPoint;
	static MObject mDispEnableFoliages;
};

using namespace MHWRender;


class SkelTreeVisualizationOverride : public MPxGeometryOverride
{
public:
	static MPxGeometryOverride* Creator(const MObject& obj) { return new SkelTreeVisualizationOverride(obj); }
	~SkelTreeVisualizationOverride() override {};

	// Support configuration functions :
	DrawAPI supportedDrawAPIs() const override { return (DrawAPI::kOpenGL | DrawAPI::kDirectX11 | DrawAPI::kOpenGLCoreProfile); }
	void updateDG() override;
	void cleanUp() override {};

	// Render item functions, only involves in foreground rendering, should not affect VP2 Caching at all
	void updateRenderItems(const MDagPath& path, MRenderItemList& list) override;

	// Geometry update functions, major entry for support VP2 Custom Caching
	bool requiresGeometryUpdate() const override;
	void populateGeometry(const MGeometryRequirements& requirements, const MRenderItemList& renderItems, MGeometry& data) override;

	static const MString sDeformedPoints;
	static const MString sChainBoxes;
	static const MString sChainLine;
	static const MString sDeformedMeshName;
	static const MString sAttachedPointName;
	static const MString sFoliageName;

private:
	SkelTreeVisualizationOverride(const MObject& obj);

	SkelTreeVisualization*		mVisNode;
	skelTree::SkelTreeDataP		pTreeData;
	skelTree::SkelTree			skelTree;

	PopulateGeometryData		popGeoData;
	DispEnableMap				visElementMap;
	RenderBufferManager			bufManager;

};