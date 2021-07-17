#pragma once

#include <RenderItemBase.h>

class RenderItemTriangles : public RenderItemBase
{
public:
	~RenderItemTriangles() override {};
	MRenderItem* create(const MString& renderItemName) override
	{
		MRenderer* renderer = MRenderer::theRenderer();
		const MShaderManager* shaderMgr = renderer->getShaderManager();

		MRenderItem* vertexItem = MRenderItem::Create(
			renderItemName,
			MRenderItem::MaterialSceneItem,
			MGeometry::kTriangles);

		vertexItem->setDrawMode(MGeometry::kAll);
		vertexItem->setExcludedFromPostEffects(false);
		vertexItem->castsShadows(true);
		vertexItem->receivesShadows(true);
		vertexItem->setSelectionMask(MSelectionMask::kSelectMeshes);

		MShaderInstance* shader = shaderMgr->getStockShader(
			MShaderManager::k3dBlinnShader);

		// Set affected color
		static const float theColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_setSolidColor(shader, theColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setSolidColor(MShaderInstance* shaderInstance, const float* value)
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "diffuseColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};