#include <SkelTreeVisualization.h>
#include <SkelTreeData.h>

#include <maya/MTypeId.h>
#include <maya/MPlug.h>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MFnPluginData.h>

MTypeId SkelTreeVisualization::id(0x20220003);
MString	SkelTreeVisualization::drawDbClassification("drawdb/geometry/skelTreeVisualization");
MString	SkelTreeVisualization::drawRegistrantId("skelTreeVisualization");

MObject SkelTreeVisualization::mInSkelTreeData;

void* SkelTreeVisualization::creator()
{
	return new SkelTreeVisualization();
}

MStatus SkelTreeVisualization::initialize()
{
	MStatus status;

	MFnTypedAttribute		tAttr;
	MFnPluginData			fnDataCreator;
	MTypeId					skelTreeDataId(SkelTreeData::id);
	mInSkelTreeData = tAttr.create("inSkelTreeData", "ist", SkelTreeData::id, fnDataCreator.create(skelTreeDataId));
	tAttr.setStorable(false);

	status = addAttribute(mInSkelTreeData);

	return status;
}

MBoundingBox SkelTreeVisualization::boundingBox() const
{
	MBoundingBox bbox;

	skelTree::CRSkelTreeData treeData = getSkelTreeData();
	skelTree::CRBox stBox = treeData.boundBox;

	bbox.expand(MPoint(stBox.min.x, stBox.min.y, stBox.min.z));
	bbox.expand(MPoint(stBox.max.x, stBox.max.y, stBox.max.z));

	return bbox;
}

skelTree::CRSkelTreeData SkelTreeVisualization::getSkelTreeData() const
{
	MObject thisNode = thisMObject();
	MPlug skelTreeDataPlug(thisNode, mInSkelTreeData);
	return ((SkelTreeData*)skelTreeDataPlug.asMDataHandle().asPluginData())->skelTreeData;
}

const MString SkelTreeVisualizationOverride::sMeshPoints = "skelTreeVisMeshPoints";

SkelTreeVisualizationOverride::SkelTreeVisualizationOverride(const MObject& obj)
	: MPxGeometryOverride(obj)
	, mVisNode(nullptr)
{
	MStatus returnStatus;
	MFnDependencyNode dependNode(obj, &returnStatus);
	if (returnStatus != MStatus::kSuccess) return;
	MPxNode* footPrintNode = dependNode.userNode(&returnStatus);
	if (returnStatus != MStatus::kSuccess) footPrintNode = nullptr;
	mVisNode = dynamic_cast<SkelTreeVisualization*>(footPrintNode);
}

void SkelTreeVisualizationOverride::updateDG()
{
	pTreeData = &mVisNode->getSkelTreeData();
}

void SkelTreeVisualizationOverride::updateRenderItems(const MDagPath& path, MRenderItemList& list)
{
	MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer();
	const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager();
	
	MHWRender::MRenderItem* vertexItem = NULL;
	int index = list.indexOf(sMeshPoints);
	if (index < 0)
	{
		vertexItem = MHWRender::MRenderItem::Create(
			sMeshPoints,
			MHWRender::MRenderItem::DecorationItem,
			MHWRender::MGeometry::kPoints);
		vertexItem->setDrawMode(MHWRender::MGeometry::kAll);
		vertexItem->depthPriority(MHWRender::MRenderItem::sDormantPointDepthPriority);
		list.append(vertexItem);

		MHWRender::MShaderInstance* shader = shaderMgr->getStockShader(
			MHWRender::MShaderManager::k3dCPVFatPointShader);

		static const float pointSize = 4.0f;
		_setSolidPointSize(shader, pointSize);

		static const float activeTemplateColor[] = { 1.0f, 0.5f, 0.5f, 1.0f };
		_setSolidColor(shader, activeTemplateColor);

		vertexItem->setShader(shader, &sMeshPoints);
		shaderMgr->releaseShader(shader);
	}
	else
	{
		vertexItem = list.itemAt(index);
	}
}

void SkelTreeVisualizationOverride::populateGeometry(const MGeometryRequirements& requirements, const MRenderItemList& renderItems, MGeometry& data)
{
	MVertexBuffer* verticesBuffer = nullptr;
	MVertexBuffer* cpvBuffer = nullptr;
	float* vertices = nullptr;
	float* cpv = nullptr;

	const MVertexBufferDescriptorList& vertexBufferDescriptorList = requirements.vertexRequirements();
	const int numberOfVertexRequirments = vertexBufferDescriptorList.length();

	const uint vertexDataSize = pTreeData->pointNum();

	// vertex buffer
	MVertexBufferDescriptor vertexBufferDescriptor;
	for (int requirmentNumber = 0; requirmentNumber < numberOfVertexRequirments; ++requirmentNumber)
	{
		if (!vertexBufferDescriptorList.getDescriptor(requirmentNumber, vertexBufferDescriptor))
		{
			continue;
		}

		switch (vertexBufferDescriptor.semantic())
		{
		case MGeometry::kPosition:
		{
			if (!verticesBuffer)
			{
				verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);
				if (verticesBuffer)
				{
					vertices = (float*)verticesBuffer->acquire(vertexDataSize, false);
				}
			}
		}
		break;
		case MGeometry::kColor:
		{
			if (!cpvBuffer)
			{
				cpvBuffer = data.createVertexBuffer(vertexBufferDescriptor);
				if (cpvBuffer)
				{
					cpv = (float*)cpvBuffer->acquire(vertexDataSize, false);
				}
			}
		}
		break;
		default:
			// do nothing for stuff we don't understand
			break;
		}
	}

	_fillMeshPoints(vertices);
	_fillMeshPointWeights(cpv);
	verticesBuffer->commit(vertices);
	cpvBuffer->commit(cpv);

	// index buffer
	for (int i = 0; i < renderItems.length(); ++i)
	{
		const MRenderItem* item = renderItems.itemAt(i);

		if (!item)
		{
			continue;
		}

		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == sMeshPoints)
		{
			int numIndex = vertexDataSize;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex, false);

			for (int i = 0; i < numIndex; ++i)
				indices[i] = i;

			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}
}

void SkelTreeVisualizationOverride::_fillMeshPoints(float* vertices)
{
	uint idx = 0;
	for(auto & deformedData : pTreeData->deformedDataList)
		for (auto& p : pTreeData->pointsList[deformedData.pointsId].finalPositions()) {
			vertices[idx++] = p.x;
			vertices[idx++] = p.y;
			vertices[idx++] = p.z;
		}
}

void SkelTreeVisualizationOverride::_fillMeshPointWeights(float* weights)
{
	uint idx = 0;
	for (auto& deformedData : pTreeData->deformedDataList)
		for (auto& w : deformedData.wList) {
			weights[idx++] = w.w[0];
			weights[idx++] = w.w[1];
			weights[idx++] = 0.0f;
		}
}

void SkelTreeVisualizationOverride::_setSolidPointSize(MHWRender::MShaderInstance* shaderInstance, float pointSize)
{
	if (!shaderInstance)
		return;

	const float pointSizeArray[2] = { pointSize, pointSize };
	const MString pointSizeParameterName = "pointSize";
	shaderInstance->setParameter(pointSizeParameterName, pointSizeArray);
}

void SkelTreeVisualizationOverride::_setSolidColor(MHWRender::MShaderInstance* shaderInstance, const float* value)
{
	if (!shaderInstance)
		return;

	const MString colorParameterName = "solidColor";
	shaderInstance->setParameter(colorParameterName, value);
}