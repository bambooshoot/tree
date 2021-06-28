#pragma once

#include <maya/MString.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MShaderManager.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>

#include <SkelTree.h>

class PopulateGeometryBase
{
public:
	virtual void  populateGeometryVertex(skelTree::SkelTreeDataP pTreeData, MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor)  = 0;
	virtual void populateGeometryIndex(skelTree::CSkelTreeDataP pTreeData, MGeometry& data, const MRenderItem* item, const MString& renderItemName) = 0;
};
