#include <AttachedLocalSpace.h>

#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnCompoundAttribute.h>
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

MTypeId AttachedLocalSpace::id(0x20220001);

MObject AttachedLocalSpace::mInMesh;
MObject AttachedLocalSpace::mInWorldMatrix;
MObject AttachedLocalSpace::mInWeight;
MObject AttachedLocalSpace::mInPntId;
MObject AttachedLocalSpace::mInPList;
MObject AttachedLocalSpace::mOutTranslate;
MObject AttachedLocalSpace::mOutRotate;

AttachedLocalSpace::AttachedLocalSpace()
{
}

AttachedLocalSpace::~AttachedLocalSpace()
{
}

MStatus AttachedLocalSpace::compute(const MPlug& plug, MDataBlock& block)
{
	if (plug != mOutRotate && plug != mOutTranslate)
		return MS::kUnknownParameter;

	MStatus status;
	MArrayDataHandle hInPList = block.inputArrayValue(mInPList, &status);

	MDataHandle hInMesh = block.inputValue(mInMesh, &status);
	if(status!=MS::kSuccess)
		return MS::kUnknownParameter;

	MFnMesh fnMesh(hInMesh.asMesh());

	unsigned int elementCount = hInPList.elementCount();

	MMatrix worldMat;
	worldMat.setToIdentity();
	if (elementCount > 0) {
		MPointArray pntArray;
		MFloatArray wArray;

		MFnDependencyNode fnMeshDgNode(thisMObject());
		MPlug inMeshPlug = fnMeshDgNode.findPlug(mInMesh);

		MDataHandle hWorldMatrix = block.inputValue(mInWorldMatrix, &status);
		MMatrix meshWorldMatrix = hWorldMatrix.asMatrix();

		MPoint pnt;
		MDataHandle hInP, hPntId, hWeight;
		for (unsigned int i = 0; i < elementCount; ++i) {
			hInPList.jumpToElement(i);
			hInP = hInPList.inputValue(&status);
			hPntId = hInP.child(mInPntId);
			hWeight = hInP.child(mInWeight);

			int pid = hPntId.asInt();
			double w = hWeight.asFloat();

			fnMesh.getPoint(pid, pnt, MSpace::kObject);
			pnt *= meshWorldMatrix;
			pntArray.append(pnt);

			wArray.append(w);
		}

		pnt.x = 0; pnt.y = 0; pnt.z = 0;
		for (unsigned int i = 0; i < pntArray.length(); ++i) {
			pnt += pntArray[i] * wArray[i];
		}

		MVector zAxis = pntArray[2] - pntArray[0];
		MVector xAxis = pntArray[1] - pntArray[0];
		xAxis.normalize();

		MVector n = xAxis ^ zAxis;
		n.normalize();

		zAxis = n ^ xAxis;
		zAxis.normalize();

		worldMat[0][0] = xAxis.x;	worldMat[0][1] = xAxis.y;	worldMat[0][2] = xAxis.z;
		worldMat[1][0] = n.x;		worldMat[1][1] = n.y;		worldMat[1][2] = n.z;
		worldMat[2][0] = zAxis.x;	worldMat[2][1] = zAxis.y;	worldMat[2][2] = zAxis.z;
		worldMat[3][0] = pnt.x;		worldMat[3][1] = pnt.y;		worldMat[3][2] = pnt.z;
	}

	MTransformationMatrix transMat(worldMat);

	MDataHandle hOutRotate = block.outputValue(mOutRotate, &status);
	MEulerRotation r = transMat.eulerRotation();
	MVector radians = r.asVector();
	radians *= 180.0 / M_PI;
	hOutRotate.set3Double(radians.x, radians.y, radians.z);

	MDataHandle hOutTranslate = block.outputValue(mOutTranslate, &status);
	MVector t = transMat.translation(MSpace::kWorld);
	hOutTranslate.set3Double(t.x, t.y, t.z);

	block.setClean(plug);

	return MS::kSuccess;
}

void* AttachedLocalSpace::creator()
{
	return new AttachedLocalSpace();
}

MStatus AttachedLocalSpace::initialize()
{
	MStatus returnStatus;
	MFnNumericAttribute  numAttr;
	MFnMatrixAttribute   mAttr;

	mOutTranslate = numAttr.create("outTranslate", "ott", MFnNumericData::k3Double, 0, &returnStatus);
	numAttr.setReadable(true);
	numAttr.setWritable(false);

	mOutRotate = numAttr.create("outRotate", "ort", MFnNumericData::k3Double, 0, &returnStatus);
	numAttr.setReadable(true);
	numAttr.setWritable(false);

	MFnTypedAttribute meshAttr;
	mInMesh = meshAttr.create("inMesh", "imh", MFnData::kMesh);
	meshAttr.setStorable(false);

	mInWorldMatrix = mAttr.create("inWorldMatrix", "iwm");
	mAttr.setStorable(false);
	mAttr.setWritable(true);
	
	mInWeight = numAttr.create("inWeight", "iw", MFnNumericData::kFloat, 0, &returnStatus);
	numAttr.setStorable(true);
	numAttr.setSoftMin(0.0f);
	numAttr.setSoftMax(1.0f);
	numAttr.setWritable(true);

	mInPntId = numAttr.create("inPntId", "pid", MFnNumericData::kLong, 0, &returnStatus);
	numAttr.setStorable(true);
	numAttr.setWritable(true);

	MFnCompoundAttribute compoundAttrib;
	mInPList = compoundAttrib.create("mInPList", "ipl");
	compoundAttrib.addChild(mInPntId);
	compoundAttrib.addChild(mInWeight);
	compoundAttrib.setArray(true);

	returnStatus = addAttribute(mOutTranslate);
	returnStatus = addAttribute(mOutRotate);
	returnStatus = addAttribute(mInMesh);
	returnStatus = addAttribute(mInWorldMatrix);
	returnStatus = addAttribute(mInPList);

	attributeAffects(mInMesh, mOutTranslate);
	attributeAffects(mInWorldMatrix, mOutTranslate);
	attributeAffects(mInWeight, mOutTranslate);
	attributeAffects(mInPntId, mOutTranslate);
	attributeAffects(mInPList, mOutTranslate);

	attributeAffects(mInMesh, mOutRotate);
	attributeAffects(mInWorldMatrix, mOutRotate);
	attributeAffects(mInWeight, mOutRotate);
	attributeAffects(mInPntId, mOutRotate);
	attributeAffects(mInPList, mOutRotate);

	return returnStatus;
}