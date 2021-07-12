#pragma once

#include <RenderItemBase.h>

class RenderItemTriangles : public RenderItemBase
{
public:
<<<<<<< Updated upstream
	MHWRender::MRenderItem* create(const MString& renderItemName) override
=======
	~RenderItemTriangles() override {};
	MHWRender::MRenderItem* create(const MString& renderItemName) const override
>>>>>>> Stashed changes
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();

		MHWRender::MRenderItem* vertexItem = MHWRender::MRenderItem::Create(
			renderItemName,
			MHWRender::MRenderItem::MaterialSceneItem,
			MHWRender::MGeometry::kTriangles);

		vertexItem->setDrawMode(MGeometry::kAll);
		vertexItem->setExcludedFromPostEffects(false);
		vertexItem->castsShadows(true);
		vertexItem->receivesShadows(true);
		vertexItem->setSelectionMask(MSelectionMask::kSelectMeshes);

		MHWRender::MShaderInstance* shader = shaderMgr->getStockShader(
			MHWRender::MShaderManager::k3dBlinnShader);

		// Set affected color
		static const float theColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_setSolidColor(shader, theColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setSolidColor(MHWRender::MShaderInstance* shaderInstance, const float* value) const
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "diffuseColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};