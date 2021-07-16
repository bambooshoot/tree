#pragma once
#include <RenderItemBase.h>

class RenderItemCPVTriangles : public RenderItemBase
{
public:
	MHWRender::MRenderItem* create(const MString& renderItemName) const override
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();

		MHWRender::MRenderItem* vertexItem = MHWRender::MRenderItem::Create(
			renderItemName,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kTriangles);

		vertexItem->setDrawMode(MGeometry::kAll);

		MHWRender::MShaderInstance* shader = shaderMgr->getStockShader(
			MHWRender::MShaderManager::k3dCPVSolidShader);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}
};