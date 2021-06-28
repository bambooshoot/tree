#pragma once

#include <maya/MPxNode.h>
#include <maya/MDataBlock.h>

#include <SkelTree.h>

class SkelTreeCreator : public MPxNode
{
public:
	SkelTreeCreator();
	~SkelTreeCreator() override;

	static void* creator();
	static MStatus initialize();

	// deformation function
	MStatus compute(const MPlug& plug, MDataBlock& data) override;

public:

	// local node attributes
	static MObject mChains;
	static MObject mAttachedPoint;
	static MObject mTargetMeshId;
	static MObject mAttachedWeights;
	static MObject mAttachedPointId;
	static MObject mAttachedWeight;

	static MObject mRootFrame;
	static MObject mOffset;
	static MObject mRootQ;

	static MObject mFrames;
	static MObject mZOffset;
	static MObject mFrameQ;

	static MObject mDeformedMeshes;
	static MObject mDeformedChainId;
	static MObject mDeformedMeshId;

	static MObject mInMeshes;
	static MObject mInMesh;
	static MObject mInWorldMatrix;

	static MObject mOutSkelTreeData;

	static MTypeId id;

private:
	void build(skelTree::SkelTreeData& skelTree, MDataBlock& block);
	void inputMeshes(skelTree::SkelTreeData& skelTree, MDataBlock& block);
	void inputChains(skelTree::SkelTreeData& skelTree, MDataBlock& block);
	void inputDeformedMeshDatas(skelTree::SkelTreeData& skelTree, MDataBlock& block);
};