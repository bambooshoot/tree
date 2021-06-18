#pragma once

#include <maya/MPxNode.h>

class AttachedLocalSpace : public MPxNode
{
public:
	AttachedLocalSpace();
	~AttachedLocalSpace() override;

	static void* creator();
	static MStatus initialize();

	// deformation function
	MStatus compute(const MPlug& plug, MDataBlock& data) override;

public:

	// local node attributes
	static MObject mInMesh;
	static MObject mInWorldMatrix;
	static MObject mInWeight;
	static MObject mInPntId;
	static MObject mInPList;
	static MObject mOutTranslate;
	static MObject mOutRotate;
	static MTypeId id;

private:
};