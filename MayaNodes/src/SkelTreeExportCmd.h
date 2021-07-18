#pragma once

#include <maya/MFnNurbsCurve.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MItCurveCV.h>
#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MPxCommand.h>
#include <maya/MArgList.h>

class SkelTreeExportCmd : public MPxCommand
{

public:
	SkelTreeExportCmd();
	~SkelTreeExportCmd() override;

	MStatus		doIt(const MArgList&) override;

	static		void* creator();
};