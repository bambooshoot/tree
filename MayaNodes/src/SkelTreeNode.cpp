#include <SkelTreeNode.h>

#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnPluginData.h>

#include <maya/MFnData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnGeometryData.h>
#include <maya/MEulerRotation.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>

#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>

#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>

#include <SkelTreeData.h>
#include <SkelPoints.h>

MTypeId SkelTree::id(0x20220002);

MObject SkelTree::mChains;

MObject SkelTree::mAttachedPoint;
MObject SkelTree::mTargetMeshId;
MObject SkelTree::mAttachedWeights;
MObject SkelTree::mAttachedPointId;
MObject SkelTree::mAttachedWeight;

MObject SkelTree::mRootFrame;
MObject SkelTree::mOffset;
MObject SkelTree::mRootQ;

MObject SkelTree::mFrames;
MObject SkelTree::mZOffset;
MObject SkelTree::mFrameQ;

MObject SkelTree::mDeformedMeshes;
MObject SkelTree::mDeformedChainId;
MObject SkelTree::mDeformedMeshId;

MObject SkelTree::mInMeshes;
MObject SkelTree::mInMesh;
MObject SkelTree::mInWorldMatrix;

MObject SkelTree::mOutSkelTree;

SkelTree::SkelTree()
{
}

SkelTree::~SkelTree()
{
}

void* SkelTree::creator()
{
	return new SkelTree();
}

MStatus SkelTree::initialize()
{
	MStatus status;

	MFnNumericAttribute		numAttr;
	MFnMatrixAttribute		mAttr;
	MFnTypedAttribute		tAttr;
	MFnCompoundAttribute	compoundAttrib;

	// in meshes
	mInMesh = tAttr.create("inMesh", "imh", MFnData::kMesh);
	tAttr.setStorable(false);

	mInWorldMatrix = mAttr.create("inWorldMatrix", "iwm");
	mAttr.setStorable(false);

	mInMeshes = compoundAttrib.create("inMeshes", "msh");
	compoundAttrib.addChild(mInMesh);
	compoundAttrib.addChild(mInWorldMatrix);
	compoundAttrib.setArray(true);
	status = addAttribute(mInMeshes);
	// in meshes

	// attached point
	mAttachedPointId = numAttr.create("attachedPointId", "apd", MFnNumericData::kInt, -1, &status);
	mAttachedWeight = numAttr.create("attachedWeight", "awt", MFnNumericData::kDouble, 0.0, &status);

	mAttachedWeights = compoundAttrib.create("attachedWeights", "ahw");
	compoundAttrib.addChild(mAttachedPointId);
	compoundAttrib.addChild(mAttachedWeight);
	compoundAttrib.setArray(true);

	mTargetMeshId = numAttr.create("targetMeshId","tmd", MFnNumericData::kInt, -1, &status);

	mAttachedPoint = compoundAttrib.create("attachedPoint", "ahp");
	compoundAttrib.addChild(mTargetMeshId);
	compoundAttrib.addChild(mAttachedWeights);
	// attached point

	// root frame
	mOffset = numAttr.create("offset", "oft", MFnNumericData::k3Double, 0.0, &status);
	mRootQ = numAttr.create("rootQuat", "rtq", MFnNumericData::k4Double, 0.0, &status);

	mRootFrame = compoundAttrib.create("rootFrame", "rtf");
	compoundAttrib.addChild(mOffset);
	compoundAttrib.addChild(mRootQ);
	// root frame

	// frame
	mZOffset = numAttr.create("zOffset", "zft", MFnNumericData::kDouble, 0.0, &status);
	mFrameQ = numAttr.create("frameQuat", "fqt", MFnNumericData::k4Double, 0.0, &status);
	mFrames = compoundAttrib.create("frames", "frm");

	compoundAttrib.addChild(mZOffset);
	compoundAttrib.addChild(mFrameQ);
	compoundAttrib.setArray(true);
	// frame

	// chains
	mChains = compoundAttrib.create("chains", "chn");
	compoundAttrib.addChild(mAttachedPoint);
	compoundAttrib.addChild(mRootFrame);
	compoundAttrib.addChild(mFrames);
	compoundAttrib.setArray(true);
	status = addAttribute(mChains);
	// chains

	// deformed meshes
	mDeformedChainId = numAttr.create("deformedChainId", "dci", MFnNumericData::kInt, -1, &status);
	mDeformedMeshId = numAttr.create("deformedMeshId", "dmi", MFnNumericData::kInt, -1, &status);
	
	mDeformedMeshes = compoundAttrib.create("deformedMeshes", "dms");
	compoundAttrib.addChild(mDeformedChainId);
	compoundAttrib.addChild(mDeformedMeshId);
	compoundAttrib.setArray(true);
	status = addAttribute(mDeformedMeshes);
	// deformed meshes

	MFnPluginData fnDataCreator;
	MTypeId skelTreeDataId(SkelTreeData::id);
	mOutSkelTree = tAttr.create("outSkelTree", "ost", SkelTreeData::id, fnDataCreator.create(skelTreeDataId));
	tAttr.setStorable(false);
	tAttr.setWritable(false);

#define ATTRIBAFFECTS(OUTATTR) \
	attributeAffects(mInMesh, OUTATTR); \
	attributeAffects(mInWorldMatrix, OUTATTR); \
	attributeAffects(mRootQ, OUTATTR); \
	attributeAffects(mFrameQ, OUTATTR);

	ATTRIBAFFECTS(mOutSkelTree)

	return status;
}

MStatus SkelTree::compute(const MPlug& plug, MDataBlock& block)
{
	MStatus status;
	
	if (plug != mOutSkelTree)
		return MS::kUnknownParameter;

	MDataHandle outSkelTreeH = block.outputValue(mOutSkelTree);
	SkelTreeData* pOutSeklTreeData = (SkelTreeData*)outSkelTreeH.asPluginData();
	skelTree::SkelTreeData & skelTreeData = pOutSeklTreeData->skelTreeData;
	build(skelTreeData, block);
	block.setClean(plug);

	return MS::kSuccess;
}

void SkelTree::build(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	inputMeshes(skelTreeData, block);
	inputChains(skelTreeData, block);
	inputDeformedMeshDatas(skelTreeData, block);
	
	skelTree::SkelTree skTree(skelTreeData);
	skTree.computWeights();
}

void SkelTree::inputMeshes(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	skelTree::Vec p;
	MPoint wP;

	MStatus status;
	MArrayDataHandle inArrayMeshesH = block.inputArrayValue(mInMeshes, &status);
	for (int i = 0; i < inArrayMeshesH.elementCount(); ++i) {
		inArrayMeshesH.jumpToElement(i);
		MDataHandle curMeshesH = inArrayMeshesH.inputValue();
		MFnMesh fnMesh(curMeshesH.child(mInMesh).asMesh());
		MMatrix matrix = curMeshesH.child(mInWorldMatrix).asMatrix();
		MPointArray pArray;
		fnMesh.getPoints(pArray);

		skelTree::RPoints curPoints = skelTreeData.addPoints();
		
		for (int pId = 0; pId < pArray.length(); ++pId) {
			skelTree::RVecList pList = curPoints.rest();
			wP = pArray[pId] * matrix;
			p.setValue(wP.x, wP.y, wP.z);
			pList.push_back(p);
		}
	}
}

void SkelTree::inputChains(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	MStatus status;
	MArrayDataHandle inArrayChainsH = block.inputArrayValue(mChains, &status);
	for (int i = 0; i < inArrayChainsH.elementCount(); ++i) {
		inArrayChainsH.jumpToElement(i);
		MDataHandle chainH = inArrayChainsH.inputValue();
		MDataHandle attachedPointH = chainH.child(mAttachedPoint);
		MDataHandle rootFrameH = chainH.child(mRootFrame);
		MArrayDataHandle framesH = chainH.child(mFrames);

		skelTree::RChainData chainData = skelTreeData.addChainData();

		// attachedPointData
		chainData.attachedPointData.pointsId = attachedPointH.child(mAttachedPointId).asInt();

		MArrayDataHandle weightsH = attachedPointH.child(mAttachedWeights);
		skelTree::RAttachedWeightList wList = chainData.attachedPointData.wList;
		skelTree::AttachedWeight weight;
		for (int i = 0; i < weightsH.elementCount(); ++i) {
			weightsH.jumpToElement(i);
			MDataHandle weightH = weightsH.inputValue();
			weight.vid = weightH.child(mAttachedPointId).asInt();
			weight.w = weightH.child(mAttachedWeight).asDouble();
			wList.push_back(weight);
		}

		// rootFrameData
		double3 &offset = rootFrameH.child(mOffset).asDouble3();
		chainData.rootFrameData.offset.setValue(offset[0], offset[1], offset[2]);

		double4 &rootQ = rootFrameH.child(mRootQ).asDouble4();
		for (int i = 0; i < 4; ++i)
			chainData.rootFrameData.q[i] = rootQ[i];

		// frameData
		skelTree::FrameData frameData;
		for (int i = 0; i < framesH.elementCount(); ++i) {
			framesH.jumpToElement(i);
			MDataHandle frameH = framesH.inputValue();
			frameData.zOffset = frameH.child(mZOffset).asDouble();

			double4& q = rootFrameH.child(mFrameQ).asDouble4();
			for (int i = 0; i < 4; ++i)
				frameData.q[i] = q[i];

			chainData.frameDataList.push_back(frameData);
		}
	}
}

void SkelTree::inputDeformedMeshDatas(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	MStatus status;
	MArrayDataHandle inDeformedMeshesH = block.inputArrayValue(mDeformedMeshes, &status);
	for (int i = 0; i < inDeformedMeshesH.elementCount(); ++i) {
		inDeformedMeshesH.jumpToElement(i);
		MDataHandle deformedMeshH = inDeformedMeshesH.inputValue();

		skelTree::RDeformedMeshData curDeformedMeshData = skelTreeData.addDeformedData();
		curDeformedMeshData.chainId = deformedMeshH.child(mDeformedChainId).asInt();
		curDeformedMeshData.pointsId = deformedMeshH.child(mDeformedMeshId).asInt();
	}
}