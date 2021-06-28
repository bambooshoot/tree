#pragma once

#include <RenderItemBase.h>

class RenderItemWireframe : public RenderItemBase
{
public:
	MHWRender::MRenderItem* create(const MString& renderItemName) override
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();

		MHWRender::MRenderItem* vertexItem = MHWRender::MRenderItem::Create(
			renderItemName,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kLines);

		vertexItem->setDrawMode(MHWRender::MGeometry::kAll);
		vertexItem->depthPriority(MHWRender::MRenderItem::sDormantPointDepthPriority);

		MHWRender::MShaderInstance* shader = shaderMgr->getStockShader(
			MHWRender::MShaderManager::k3dThickLineShader);

		// Set lines a bit thicker to stand out
		static const float lineSize = 5.0f;
		_setLineWidth(shader, lineSize);

		// Set affected color
		static const float theColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		_setSolidColor(shader, theColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setLineWidth(MHWRender::MShaderInstance* shaderInstance, float lineWidth)
	{
		if (!shaderInstance)
			return;

		const float lineWidthArray[2] = { lineWidth, lineWidth };
		const MString lineWidthParameterName = "lineWidth";
		shaderInstance->setParameter(lineWidthParameterName, lineWidthArray);
	}

	void _setSolidColor(MHWRender::MShaderInstance* shaderInstance, const float* value)
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "solidColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};