#pragma once

#include <RenderItemBase.h>

class RenderItemCPVPoint : public RenderItemBase
{
public:
	~RenderItemCPVPoint() override {};
	MHWRender::MRenderItem* create(const MString& renderItemName) override
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
		const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();

		MHWRender::MRenderItem* vertexItem = MHWRender::MRenderItem::Create(
			renderItemName,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kPoints);

		vertexItem->setDrawMode(MHWRender::MGeometry::kAll);
		vertexItem->depthPriority(MHWRender::MRenderItem::sDormantPointDepthPriority);

		MHWRender::MShaderInstance* shader = shaderMgr->getStockShader(
			MHWRender::MShaderManager::k3dCPVFatPointShader);

		static const float pointSize = 4.0f;
		_setSolidPointSize(shader, pointSize);

		static const float activeTemplateColor[] = { 1.0f, 0.5f, 0.5f, 1.0f };
		_setSolidColor(shader, activeTemplateColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setSolidPointSize(MHWRender::MShaderInstance* shaderInstance, float pointSize)
	{
		if (!shaderInstance)
			return;

		const float pointSizeArray[2] = { pointSize, pointSize };
		const MString pointSizeParameterName = "pointSize";
		shaderInstance->setParameter(pointSizeParameterName, pointSizeArray);
	}

	void _setSolidColor(MHWRender::MShaderInstance* shaderInstance, const float* value)
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "solidColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};