#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryDeformedPoints : public PopulateGeometryBase
{
public:
	void populateGeometryVertex(skelTree::SkelTreeDataP pTreeData, MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor) override
	{
		uint vertexDataSize = pTreeData->deformedPointNum();
		switch (vertexBufferDescriptor.semantic())
		{
		case MGeometry::kPosition:
		{
			MVertexBuffer* verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			if (verticesBuffer)
			{
				float* vertices = (float*)verticesBuffer->acquire(vertexDataSize, true);
				_fillMeshPoints(pTreeData, vertices);
				verticesBuffer->commit(vertices);
			}
		}
		break;
		case MGeometry::kColor:
		{
			MVertexBuffer* cpvBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			if (cpvBuffer)
			{
				float* cpv = (float*)cpvBuffer->acquire(vertexDataSize, true);
				_fillMeshPointWeights(pTreeData, cpv);
				cpvBuffer->commit(cpv);
			}
		}
		break;
		default:
			// do nothing for stuff we don't understand
			break;
		}
	}

	void populateGeometryIndex(skelTree::CSkelTreeDataP pTreeData, MGeometry& data, const MRenderItem* item, const MString& renderItemName) override
	{
		uint vertexDataSize = pTreeData->deformedPointNum();
		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == renderItemName)
		{
			int numIndex = vertexDataSize;

			unsigned int* indices = (unsigned int*)indexBuffer->acquire(numIndex, true);

			for (int i = 0; i < numIndex; ++i)
				indices[i] = i;

			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}

private:

	void _fillMeshPoints(skelTree::CSkelTreeDataP pTreeData, float* vertices)
	{
		uint idx = 0;
		for (auto& deformedData : pTreeData->deformedDataList)
			for (auto& p : pTreeData->pointsList[deformedData.pointsId].finalPositions()) {
				vertices[idx++] = p.x;
				vertices[idx++] = p.y;
				vertices[idx++] = p.z;
			}
	}

	void _fillMeshPointWeights(skelTree::CSkelTreeDataP pTreeData, float* weights)
	{
		uint idx = 0;
		for (auto& deformedData : pTreeData->deformedDataList)
			for (auto& w : deformedData.wList) {
				weights[idx++] = 1.0f;
				weights[idx++] = 1.0f;
				weights[idx++] = 0.0f;
				weights[idx++] = 1.0f;
			}
	}
};