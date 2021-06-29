#include <SkelTreeCreatorNode.h>

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
#include <maya/MGlobal.h>

#include <SkelTreeData.h>
#include <SkelPoints.h>

MTypeId SkelTreeCreator::id(0x20220002);

MObject SkelTreeCreator::mChains;

MObject SkelTreeCreator::mAttachedPoint;
MObject SkelTreeCreator::mTargetMeshId;
MObject SkelTreeCreator::mAttachedWeights;
MObject SkelTreeCreator::mAttachedPointId;
MObject SkelTreeCreator::mAttachedWeight;

MObject SkelTreeCreator::mRootFrame;
MObject SkelTreeCreator::mOffset;
MObject SkelTreeCreator::mRootQ;

MObject SkelTreeCreator::mFrames;
MObject SkelTreeCreator::mXOffset;
MObject SkelTreeCreator::mFrameQ;

MObject SkelTreeCreator::mDeformedMeshes;
MObject SkelTreeCreator::mDeformedChainId;
MObject SkelTreeCreator::mDeformedMeshId;

MObject SkelTreeCreator::mInMeshes;
MObject SkelTreeCreator::mInMesh;
MObject SkelTreeCreator::mInWorldMatrix;

MObject SkelTreeCreator::mOutSkelTreeData;

SkelTreeCreator::SkelTreeCreator()
{
}

SkelTreeCreator::~SkelTreeCreator()
{
}

void* SkelTreeCreator::creator()
{
	return new SkelTreeCreator();
}

MStatus SkelTreeCreator::initialize()
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
	mXOffset = numAttr.create("xOffset", "xft", MFnNumericData::kDouble, 0.0, &status);
	mFrameQ = numAttr.create("frameQuat", "fqt", MFnNumericData::k4Double, 0.0, &status);
	mFrames = compoundAttrib.create("frames", "frm");

	compoundAttrib.addChild(mXOffset);
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
	mOutSkelTreeData = tAttr.create("outSkelTreeData", "ost", SkelTreeData::id, fnDataCreator.create(skelTreeDataId));
	tAttr.setStorable(false);
	tAttr.setWritable(false);
	status = addAttribute(mOutSkelTreeData);

#define ATTRIBAFFECTS(OUTATTR) \
	attributeAffects(mInMesh, OUTATTR); \
	attributeAffects(mInWorldMatrix, OUTATTR); \
	attributeAffects(mRootQ, OUTATTR); \
	attributeAffects(mFrameQ, OUTATTR);

	ATTRIBAFFECTS(mOutSkelTreeData)

	return status;
}

MStatus SkelTreeCreator::compute(const MPlug& plug, MDataBlock& block)
{
	MStatus status;
	
	if (plug != mOutSkelTreeData)
		return MS::kUnknownParameter;

	MDataHandle outSkelTreeH = block.outputValue(mOutSkelTreeData);
	SkelTreeData* pOutSeklTreeData = (SkelTreeData*)outSkelTreeH.asPluginData();
	skelTree::SkelTreeData & skelTreeData = pOutSeklTreeData->skelTreeData;
	build(skelTreeData, block);
	block.setClean(plug);

	return MS::kSuccess;
}

void SkelTreeCreator::build(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	skelTreeData.reset();
	inputMeshes(skelTreeData, block);
	inputChains(skelTreeData, block);
	inputDeformedMeshDatas(skelTreeData, block);
	skelTreeData.computeBox();
	
	skelTree::SkelTree skTree(&skelTreeData);
	skTree.buildChains();
	skTree.computWeights();

	for (uint chainId = 0; chainId < skTree.chainNum(); ++chainId) {
		skelTree::CRChain chain = skTree.getChain(chainId);
		for (uint spaceId = 0; spaceId < chain.spaceNum(); ++spaceId) {
			skelTree::CRMatrix44 mat = chain.space(spaceId)->restMatrix();
			MString info;
			info += spaceId;
			info += "\n";
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					info += mat[i][j];
					info += " ";
				}
				info += "\n";
			}
					
			MGlobal::displayInfo(info);
		}
	}
}

void SkelTreeCreator::inputMeshes(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	skelTree::Vec p;
	MPoint wP;

	MStatus status;
	MArrayDataHandle inArrayMeshesH = block.inputArrayValue(mInMeshes, &status);
	for (uint i = 0; i < inArrayMeshesH.elementCount(); ++i) {
		inArrayMeshesH.jumpToElement(i);
		MDataHandle curMeshesH = inArrayMeshesH.inputValue();
		MFnMesh fnMesh(curMeshesH.child(mInMesh).asMesh());
		MMatrix matrix = curMeshesH.child(mInWorldMatrix).asMatrix();
		MPointArray pArray;
		fnMesh.getPoints(pArray);

		skelTree::RPoints curPoints = skelTreeData.addPoints();
		
		for (uint pId = 0; pId < pArray.length(); ++pId) {
			skelTree::RVecList pList = curPoints.rest();
			wP = pArray[pId] * matrix;
			p.setValue(wP.x, wP.y, wP.z);
			pList.push_back(p);
		}
	}
}

void SkelTreeCreator::inputChains(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	MStatus status;
	MArrayDataHandle inArrayChainsH = block.inputArrayValue(mChains, &status);
	for (uint chainId = 0; chainId < inArrayChainsH.elementCount(); ++chainId) {
		inArrayChainsH.jumpToElement(chainId);
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
		for (uint i = 0; i < weightsH.elementCount(); ++i) {
			weightsH.jumpToElement(i);
			MDataHandle weightH = weightsH.inputValue();
			weight.vid = weightH.child(mAttachedPointId).asInt();
			weight.w = float(weightH.child(mAttachedWeight).asDouble());
			wList.push_back(weight);
		}

		// rootFrameData
		double3 &offset = rootFrameH.child(mOffset).asDouble3();
		chainData.rootFrameData.offset.setValue(offset[0], offset[1], offset[2]);

		double4 &rootQ = rootFrameH.child(mRootQ).asDouble4();
		chainData.rootFrameData.q.r = float(rootQ[3]);
		chainData.rootFrameData.q.v.x = float(rootQ[0]);
		chainData.rootFrameData.q.v.y = float(rootQ[1]);
		chainData.rootFrameData.q.v.z = float(rootQ[2]);

		MString info("root frame data");
		info += "\n";
		info += offset[0];
		info += " ";
		info += offset[1];
		info += " ";
		info += offset[2];
		info += "\n";
		for (int i = 0; i < 4; ++i) {
			info += rootQ[i];
			info += " ";
		}
		info += "\n";
		MGlobal::displayInfo(info);

		// frameData
		skelTree::FrameData frameData;
		for (uint i = 0; i < framesH.elementCount(); ++i) {
			framesH.jumpToElement(i);
			MDataHandle frameH = framesH.inputValue();
			frameData.xOffset = float(frameH.child(mXOffset).asDouble());
			MString info("frame data");
			info += i;
			info += "\n";
			info += frameData.xOffset;

			double4& q = frameH.child(mFrameQ).asDouble4();
			frameData.q.r = float(q[3]);
			frameData.q.v.x = float(q[0]);
			frameData.q.v.y = float(q[1]);
			frameData.q.v.z = float(q[2]);

			for (int ii = 0; ii < 4; ++ii) {
				info += " ";
				info += q[ii];
			}

			MGlobal::displayInfo(info);

			chainData.frameDataList.push_back(frameData);
		}
	}
}

void SkelTreeCreator::inputDeformedMeshDatas(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	MStatus status;
	MArrayDataHandle inDeformedMeshesH = block.inputArrayValue(mDeformedMeshes, &status);
	for (uint i = 0; i < inDeformedMeshesH.elementCount(); ++i) {
		inDeformedMeshesH.jumpToElement(i);
		MDataHandle deformedMeshH = inDeformedMeshesH.inputValue();

		skelTree::RDeformedMeshData curDeformedMeshData = skelTreeData.addDeformedData();
		curDeformedMeshData.chainId = deformedMeshH.child(mDeformedChainId).asInt();
		curDeformedMeshData.pointsId = deformedMeshH.child(mDeformedMeshId).asInt();
	}
}