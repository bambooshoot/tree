#include <RenderBuffer.h>
#include <SkelTreeVisualization.h>

void RenderBufferManager::_build()
{
	RenderItemBase* pRenderItem = nullptr;
	DrawableBufferBase* pDrawBuf = nullptr;

	// draw chain boxes
	pRenderItem = new RenderItemCPVTriangles();
	pDrawBuf = new DrawableBufferChainBoxes();
	registerBuffer(SkelTreeVisualizationOverride::sChainBoxes, pRenderItem, pDrawBuf);

	// draw deformed points
	pRenderItem = new RenderItemCPVPoint();
	pDrawBuf = new DrawableBufferDeformedPoints();
	registerBuffer(SkelTreeVisualizationOverride::sDeformedPoints, pRenderItem, pDrawBuf);

	// draw chain lines
	pRenderItem = new RenderItemWireframe();
	pDrawBuf = new DrawableBufferChainLine();
	registerBuffer(SkelTreeVisualizationOverride::sChainLine, pRenderItem, pDrawBuf);

	// draw deformed triangles
	pRenderItem = new RenderItemTriangles();
	pDrawBuf = new DrawableBufferMesh();
	registerBuffer(SkelTreeVisualizationOverride::sDeformedMeshName, pRenderItem, pDrawBuf);

	// draw attached point
	pRenderItem = new RenderItemWireframe();
	pDrawBuf = new DrawableBufferAttachedPoint();
	registerBuffer(SkelTreeVisualizationOverride::sAttachedPointName, pRenderItem, pDrawBuf);

	// draw foliages
	pRenderItem = new RenderItemTriangles();
	pDrawBuf = new DrawableBufferFoliages();
	registerBuffer(SkelTreeVisualizationOverride::sFoliageName, pRenderItem, pDrawBuf);

	// draw noise graph
	pRenderItem = new RenderItemCPVPoint();
	pDrawBuf = new DrawableBufferNoiseGraph();
	registerBuffer(SkelTreeVisualizationOverride::sNoiseGraph, pRenderItem, pDrawBuf);
}