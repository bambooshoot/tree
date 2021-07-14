#pragma once

#include <maya/MString.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MShaderManager.h>

using namespace MHWRender;

class RenderItemBase
{
public:
	virtual ~RenderItemBase() {};
	virtual MHWRender::MRenderItem* create(const MString& renderItemName) = 0;
};

using RenderItemBasePtr = std::unique_ptr<RenderItemBase>;