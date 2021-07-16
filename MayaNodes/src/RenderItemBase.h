#pragma once

#include <maya/MString.h>
#include <maya/MShaderManager.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MShaderManager.h>
#include <maya/MSelectionMask.h>

using namespace MHWRender;

class RenderItemBase
{
public:
<<<<<<< Updated upstream
	virtual MHWRender::MRenderItem* create(const MString& renderItemName) = 0;
=======
	virtual ~RenderItemBase() {};
	virtual MHWRender::MRenderItem* create(const MString& renderItemName) const = 0;
>>>>>>> Stashed changes
};
