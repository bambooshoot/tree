#pragma once

#include <RenderItemBase.h>

class RenderItemWireframe : public RenderItemBase
{
public:
	~RenderItemWireframe() override {};
	MRenderItem* create(const MString& renderItemName) override
	{
		MRenderer* renderer = MRenderer::theRenderer();
		const MShaderManager* shaderMgr = renderer->getShaderManager();

		MRenderItem* vertexItem = MRenderItem::Create(
			renderItemName,
			MRenderItem::DecorationItem,
			MGeometry::kLines);

		vertexItem->setDrawMode(MGeometry::kAll);
		vertexItem->depthPriority(MRenderItem::sDormantPointDepthPriority);

		MShaderInstance* shader = shaderMgr->getStockShader(
			MShaderManager::k3dCPVThickLineShader);

		// Set lines a bit thicker to stand out
		static const float lineSize = 3.0f;
		_setLineWidth(shader, lineSize);

		// Set affected color
		static const float theColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_setSolidColor(shader, theColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setLineWidth(MShaderInstance* shaderInstance, float lineWidth)
	{
		if (!shaderInstance)
			return;

		const float lineWidthArray[2] = { lineWidth, lineWidth };
		const MString lineWidthParameterName = "lineWidth";
		shaderInstance->setParameter(lineWidthParameterName, lineWidthArray);
	}

	void _setSolidColor(MShaderInstance* shaderInstance, const float* value)
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "solidColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};