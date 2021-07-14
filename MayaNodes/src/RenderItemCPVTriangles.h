#pragma once
#include <RenderItemBase.h>

class RenderItemCPVTriangles : public RenderItemBase
{
public:
	MRenderItem* create(const MString& renderItemName) override
	{
		MRenderer* renderer = MRenderer::theRenderer();
		const MShaderManager* shaderMgr = renderer->getShaderManager();

		MRenderItem* vertexItem = MRenderItem::Create(
			renderItemName,
			MRenderItem::DecorationItem,
			MGeometry::kTriangles);

		vertexItem->setDrawMode(MGeometry::kAll);

		MShaderInstance* shader = shaderMgr->getStockShader(
			MShaderManager::k3dCPVSolidShader);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}
};