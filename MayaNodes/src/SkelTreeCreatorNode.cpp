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
#include <maya/MFnIntArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnArrayAttrsData.h>

#include <maya/MFloatPoint.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>

#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFloatArray.h>
#include <maya/MGlobal.h>

#include <SkelTreeNodeData.h>
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
MObject SkelTreeCreator::mDeformedPointIds;
MObject SkelTreeCreator::mDeformedMeshId;

MObject SkelTreeCreator::mFoliages;
MObject SkelTreeCreator::mFoliageMeshId;
MObject SkelTreeCreator::mFoliageAttachedMeshId;
MObject SkelTreeCreator::mFoliageAttachedPointId;
MObject SkelTreeCreator::mFoliageAttachedWeight;
MObject SkelTreeCreator::mFoliageQ;

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
	mAttachedPointId = numAttr.create("attachedPointId", "apd", MFnNumericData::k3Int, -1, &status);
	mAttachedWeight = numAttr.create("attachedWeight", "awt", MFnNumericData::k2Double, 0.0, &status);

	mTargetMeshId = numAttr.create("targetMeshId","tmd", MFnNumericData::kInt, -1, &status);

	mAttachedPoint = compoundAttrib.create("attachedPoint", "ahp");
	compoundAttrib.addChild(mTargetMeshId);
	compoundAttrib.addChild(mAttachedPointId);
	compoundAttrib.addChild(mAttachedWeight);
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
	mDeformedPointIds = tAttr.create("deformedPointIds", "dpi", MFnData::kIntArray, &status);
	
	mDeformedMeshes = compoundAttrib.create("deformedMeshes", "dms");
	compoundAttrib.addChild(mDeformedChainId);
	compoundAttrib.addChild(mDeformedMeshId);
	compoundAttrib.addChild(mDeformedPointIds);
	compoundAttrib.setArray(true);
	status = addAttribute(mDeformedMeshes);
	// deformed meshes

	// foliages
	mFoliageMeshId = numAttr.create("foliageMeshId", "fmd", MFnNumericData::kInt, -1, &status);
	mFoliageAttachedMeshId = numAttr.create("foliageAttachedMeshId", "fad", MFnNumericData::kInt, -1, &status);
	mFoliageAttachedPointId = tAttr.create("foliageAttachedPointId", "fpd", MFnData::kIntArray, &status);
	mFoliageAttachedWeight = tAttr.create("foliageAttachedWeight", "fwt", MFnData::kDoubleArray, &status);

	mFoliageQ = tAttr.create("foliageQuat", "foq", MFnData::kDoubleArray, &status);

	mFoliages = compoundAttrib.create("foliages", "flg");
	compoundAttrib.addChild(mFoliageMeshId);
	compoundAttrib.addChild(mFoliageAttachedMeshId);
	compoundAttrib.addChild(mFoliageAttachedPointId);
	compoundAttrib.addChild(mFoliageAttachedWeight);
	compoundAttrib.addChild(mFoliageQ);

	status = addAttribute(mFoliages);
	// foliages

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
	inputFoliages(skelTreeData, block);
	skelTreeData.computeBox();
	
	skelTree::SkelTree skTree(&skelTreeData);
	skTree.buildRest(&skelTreeData);
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
		chainData.attachedPointData.pointsId = attachedPointH.child(mTargetMeshId).asInt();
		skelTree::RAttachedPointData apd = chainData.attachedPointData;

		int3& id3 = attachedPointH.child(mAttachedPointId).asInt3();
		apd.vid[0] = id3[0];
		apd.vid[1] = id3[1];
		apd.vid[2] = id3[2];

		double2& d2 = attachedPointH.child(mAttachedWeight).asDouble2();
		apd.w[0] = float(d2[0]);
		apd.w[1] = float(d2[1]);

		// rootFrameData
		double3 &offset = rootFrameH.child(mOffset).asDouble3();
		chainData.rootFrameData.offset.setValue(offset[0], offset[1], offset[2]);

		double4 &rootQ = rootFrameH.child(mRootQ).asDouble4();
		chainData.rootFrameData.q.r = float(rootQ[3]);
		chainData.rootFrameData.q.v.x = float(rootQ[0]);
		chainData.rootFrameData.q.v.y = float(rootQ[1]);
		chainData.rootFrameData.q.v.z = float(rootQ[2]);

		// frameData
		skelTree::FrameData frameData;
		for (uint i = 0; i < framesH.elementCount(); ++i) {
			framesH.jumpToElement(i);
			MDataHandle frameH = framesH.inputValue();
			frameData.xOffset = float(frameH.child(mXOffset).asDouble());

			double4& q = frameH.child(mFrameQ).asDouble4();
			frameData.q.r = float(q[3]);
			frameData.q.v.x = float(q[0]);
			frameData.q.v.y = float(q[1]);
			frameData.q.v.z = float(q[2]);

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
		MFnIntArrayData fnPidData(deformedMeshH.child(mDeformedPointIds).data());
		MIntArray pIdArray = fnPidData.array();
		curDeformedMeshData.wList.resize(pIdArray.length());
		for (uint j = 0; j < pIdArray.length(); ++j) {
			curDeformedMeshData.wList[j].pId = pIdArray[j];
		}
	}
}

void SkelTreeCreator::inputFoliages(skelTree::SkelTreeData& skelTreeData, MDataBlock& block)
{
	MStatus status;
	MDataHandle foliageH = block.inputValue(mFoliages, &status);

	uint meshId = foliageH.child(mFoliageMeshId).asInt();
	uint attachedMeshId = foliageH.child(mFoliageAttachedMeshId).asInt();

	MFnIntArrayData fnPidData(foliageH.child(mFoliageAttachedPointId).data());
	MIntArray pIdArray = fnPidData.array();

	MFnDoubleArrayData fnWeightData(foliageH.child(mFoliageAttachedWeight).data());
	MDoubleArray weightArray = fnWeightData.array();

	MFnDoubleArrayData fnQData(foliageH.child(mFoliageQ).data());
	MDoubleArray qArray = fnQData.array();

	int attachedPointNum = pIdArray.length() / 3;
	uint pId3 = 0, wId2 = 0, qId4=0;
	for (int i = 0; i < attachedPointNum; ++i) {
		skelTree::RFoliageData foliageData = skelTreeData.addFoliageData();
		foliageData.pointsId = meshId;
		foliageData.attachPoint.pointsId = attachedMeshId;
		
		foliageData.attachPoint.vid[0] = pIdArray[pId3++];
		foliageData.attachPoint.vid[1] = pIdArray[pId3++];
		foliageData.attachPoint.vid[2] = pIdArray[pId3++];

		foliageData.attachPoint.w[0] = float(weightArray[wId2++]);
		foliageData.attachPoint.w[1] = float(weightArray[wId2++]);

		foliageData.q.v.x = float(qArray[qId4++]);
		foliageData.q.v.y = float(qArray[qId4++]);
		foliageData.q.v.z = float(qArray[qId4++]);
		foliageData.q.r = float(qArray[qId4++]);
	}
}