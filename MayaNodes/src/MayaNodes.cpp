// MayaNodes.cpp
// author : zhusun
//
// AtachedLocalSpace
//		A dg node to get a local space attached on a polygon of a mesh by input polygon id and weights

#include <AttachedLocalSpace.h>

#include <maya/MFnPlugin.h>


////////////////////////////////////////////////////////////////////////
//
// Standard Initialization Procedures
//
////////////////////////////////////////////////////////////////////////

#define PLUGIN_COMPANY "zhusun"

MStatus initializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj, PLUGIN_COMPANY, "3.0", "Any");

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

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus   status;
    MFnPlugin plugin(obj);

    status = plugin.deregisterNode(AttachedLocalSpace::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }
    return status;
}
