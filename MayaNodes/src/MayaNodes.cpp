// MayaNodes.cpp
// author : zhusun
//
// AtachedLocalSpace
//		A dg node to get a local space attached on a polygon of a mesh by input polygon id and weights

#include <AttachedLocalSpace.h>
#include <SkelTreeCreatorNode.h>
#include <SkelTreeVisualization.h>
#include <SkelTreeNodeData.h>
#include <SkelTreeExportCmd.h>

#include <maya/MFnPlugin.h>


////////////////////////////////////////////////////////////////////////
//
// Standard Initialization Procedures
//
////////////////////////////////////////////////////////////////////////

#define AUTHOR "zhusun"

MStatus initializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj, AUTHOR, "3.0", "Any");

    status = plugin.registerData("skelTreeData", SkelTreeData::id,
        &SkelTreeData::creator,
        MPxData::kData);

    status = plugin.registerCommand("skelTreeExport", SkelTreeExportCmd::creator);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

    status = plugin.registerNode(
        "attachedLocalSpace",
        AttachedLocalSpace::id,
        &AttachedLocalSpace::creator,
        &AttachedLocalSpace::initialize,
        MPxNode::kDependNode);

    if (!status) {
        status.perror("registerNode attachedLocalSpace");
        return status;
    }

    status = plugin.registerNode("skelTreeCreator",
        SkelTreeCreator::id,
        SkelTreeCreator::creator,
        SkelTreeCreator::initialize,
        MPxNode::kDependNode);

    if (!status) {
        status.perror("registerNode skelTreeCreator");
        return status;
    }

    status = plugin.registerNode("skelTreeVisualization",
        SkelTreeVisualization::id,
        SkelTreeVisualization::creator,
        SkelTreeVisualization::initialize,
        MPxNode::kLocatorNode,
        &SkelTreeVisualization::drawDbClassification);

    if (!status) {
        status.perror("registerNode skelTreeVisualization");
        return status;
    }

    status = MDrawRegistry::registerSubSceneOverrideCreator(
        SkelTreeVisualization::drawDbClassification,
        SkelTreeVisualization::drawRegistrantId,
        SkelTreeVisualizationOverride::Creator);

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterData(SkelTreeData::id);

    status = plugin.deregisterCommand("skelTreeExport");
    if (!status) {
        status.perror("deregisterCommand");
        return status;
    }

    status = plugin.deregisterNode(AttachedLocalSpace::id);
    if (!status) {
        status.perror("deregisterNode attachedLocalSpace");
        return status;
    }

    status = plugin.deregisterNode(SkelTreeCreator::id);
    if (!status) {
        status.perror("deregisterNode skelTreeCreator");
        return status;
    }

    status = plugin.deregisterNode(SkelTreeVisualization::id);
    if (!status) {
        status.perror("deregisterNode skelTreeVisualization");
        return status;
    }

    MDrawRegistry::deregisterSubSceneOverrideCreator(
        SkelTreeVisualization::drawDbClassification,
        SkelTreeVisualization::drawRegistrantId);

    return status;
}
