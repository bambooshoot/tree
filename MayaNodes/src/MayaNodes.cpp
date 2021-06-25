// MayaNodes.cpp
// author : zhusun
//
// AtachedLocalSpace
//		A dg node to get a local space attached on a polygon of a mesh by input polygon id and weights

#include <AttachedLocalSpace.h>
#include <SkelTreeNode.h>
#include <SkelTreeData.h>

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

    status = plugin.registerData("nurbsHairData", SkelTreeData::id,
        &SkelTreeData::creator,
        MPxData::kData);

    status = plugin.registerNode(
        "attachedLocalSpace",
        AttachedLocalSpace::id,
        &AttachedLocalSpace::creator,
        &AttachedLocalSpace::initialize,
        MPxNode::kDependNode);

    if (!status) {
        status.perror("registerNode");
        return status;
    }

    status = plugin.registerNode("skelTree",
        SkelTree::id,
        SkelTree::creator,
        SkelTree::initialize,
        MPxNode::kLocatorNode);

    if (!status) {
        status.perror("registerNode");
        return status;
    }

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterData(SkelTreeData::id);

    status = plugin.deregisterNode(AttachedLocalSpace::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    status = plugin.deregisterNode(SkelTree::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    return status;
}
