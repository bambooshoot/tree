#pragma once

#include <maya/MPxLocatorNode.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MDataBlock.h>

#include <SkelTree.h>

class SkelTreeVisualization : public MPxLocatorNode
{
public:
	SkelTreeVisualization() {};
	~SkelTreeVisualization() override {};

	//bool setInternalValue(const MPlug&, const MDataHandle&) override;

	static  void*   creator();
	static  MStatus initialize();

	skelTree::CRSkelTreeData getSkelTreeData() const;

	//bool            excludeAsLocator() const override;
	//void            postConstructor() override;

	bool            isTransparent() const override { return true; }
	bool            drawLast() const override { return true; }
	bool            isBounded() const override { return true; }
	MBoundingBox    boundingBox() const override;

	static MTypeId id;
	static MString drawDbClassification;
	static MString drawRegistrantId;

	static MObject mInSkelTreeData;
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
	//bool requiresGeometryUpdate() const override;
	void populateGeometry(const MGeometryRequirements& requirements, const MRenderItemList& renderItems, MGeometry& data) override;

private:
	SkelTreeVisualizationOverride(const MObject& obj);
	void _fillMeshPoints(float * vertices);
	void _fillMeshPointWeights(float * weights);
	void _setSolidPointSize(MHWRender::MShaderInstance* shaderInstance, float pointSize);
	void _setSolidColor(MHWRender::MShaderInstance* shaderInstance, const float* value);

	SkelTreeVisualization*		mVisNode;
	skelTree::CSkelTreeDataP	pTreeData;

	static const MString sMeshPoints;
};