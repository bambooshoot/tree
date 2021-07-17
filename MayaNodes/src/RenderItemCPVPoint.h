#pragma once

#include <RenderItemBase.h>

class RenderItemCPVPoint : public RenderItemBase
{
public:
	~RenderItemCPVPoint() override {};
	MRenderItem* create(const MString& renderItemName) override
	{
		MRenderer* renderer = MRenderer::theRenderer();
		const MShaderManager* shaderMgr = renderer->getShaderManager();

		MRenderItem* vertexItem = MRenderItem::Create(
			renderItemName,
			MRenderItem::DecorationItem,
			MGeometry::kPoints);

		vertexItem->setDrawMode(MGeometry::kAll);
		vertexItem->depthPriority(MRenderItem::sDormantPointDepthPriority);

		MShaderInstance* shader = shaderMgr->getStockShader(
			MShaderManager::k3dCPVFatPointShader);

		static const float pointSize = 4.0f;
		_setSolidPointSize(shader, pointSize);

		static const float activeTemplateColor[] = { 1.0f, 0.5f, 0.5f, 1.0f };
		_setSolidColor(shader, activeTemplateColor);

		vertexItem->setShader(shader, &renderItemName);
		shaderMgr->releaseShader(shader);

		return vertexItem;
	}

private:
	void _setSolidPointSize(MShaderInstance* shaderInstance, float pointSize)
	{
		if (!shaderInstance)
			return;

		const float pointSizeArray[2] = { pointSize, pointSize };
		const MString pointSizeParameterName = "pointSize";
		shaderInstance->setParameter(pointSizeParameterName, pointSizeArray);
	}

	void _setSolidColor(MShaderInstance* shaderInstance, const float* value)
	{
		if (!shaderInstance)
			return;

		const MString colorParameterName = "solidColor";
		shaderInstance->setParameter(colorParameterName, value);
	}
};