#include <SkelTreeExportCmd.h>
#include <SkelTreeVisualization.h>
#include <SkelTreeFile.h>

SkelTreeExportCmd::SkelTreeExportCmd() {};
SkelTreeExportCmd::~SkelTreeExportCmd() {};

void* SkelTreeExportCmd::creator()
{
	return new SkelTreeExportCmd();
}

MStatus SkelTreeExportCmd::doIt(const MArgList& args)
{
	MStatus status;

	MString fileName;
	MString skelTreeVisNode;

	// Parse the arguments.
	for (unsigned i = 0; i < args.length(); i++) {
		if (MString("-f") == args.asString(i, &status)
			&& MS::kSuccess == status) {
			fileName = args.asString(++i, &status);
		}else if (MString("-n") == args.asString(i, &status)
			&& MS::kSuccess == status) {
			skelTreeVisNode = args.asString(++i, &status);
		}
	}

	MSelectionList slist;
	MGlobal::getSelectionListByName(skelTreeVisNode, slist);
	uint matchedObjNum = slist.length();
	if (matchedObjNum == 0) {
		MGlobal::displayWarning("SkelTree Visualization Node input does not exist, check the node name please.");
		return MS::kFailure;
	}

	MObject depNode;
	status = slist.getDependNode(0, depNode);

	MFnDependencyNode fnDepNode(depNode);
	MPxNode* userNode = fnDepNode.userNode(&status);
	if (status != MStatus::kSuccess) {
		MGlobal::displayWarning("The Node input is not a SkelTree Visualization Node.");
	}

	SkelTreeVisualization* pVisNode = dynamic_cast<SkelTreeVisualization*>(userNode);
	skelTree::RSkelTreeData treeData = pVisNode->getSkelTreeData();
	skelTree::AniOpData aniOpData = pVisNode->aniOpData();

	skelTree::File::write(treeData, aniOpData, fileName.asChar());
	MGlobal::displayInfo("SkelTree exported.");
	MGlobal::displayInfo(skelTreeVisNode);
	MGlobal::displayInfo(fileName);

	return status;
}