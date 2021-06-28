#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryChains : public PopulateGeometryBase
{
public:
	void populateGeometryVertex(skelTree::SkelTreeDataP pTreeData, MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor) override
	{
		skTree.reset(pTreeData);
		skTree.buildChains();

		uint chainNum = skTree.spaceNum() * 4;

		switch (vertexBufferDescriptor.semantic())
		{
		case MGeometry::kPosition:
		{
			MVertexBuffer* verticesBuffer = data.createVertexBuffer(vertexBufferDescriptor);
			if (verticesBuffer)
			{
				float* vertices = (float*)verticesBuffer->acquire(chainNum, true);
				_fillPoints(vertices);
				verticesBuffer->commit(vertices);
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
		uint chainNum = skTree.spaceNum() * 6;
		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == renderItemName)
		{
			unsigned int* indices = (unsigned int*)indexBuffer->acquire(chainNum, true);
			_fillIndex(indices);
			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}

private:
	skelTree::SkelTree skTree;

	void _fillPoints(float* vertices)
	{
		uint idx = 0;
		skelTree::Vec p4[4] = { 
			skelTree::Vec(0, 0, 0), 
			skelTree::Vec(1, 0, 0), 
			skelTree::Vec(0, 1, 0), 
			skelTree::Vec(0, 0, 1)
		}, p1;

		for (uint i = 0; i < skTree.chainNum(); ++i) {
			skelTree::CRChain chain = skTree.getChain(i);
			for (uint frameId = 0; frameId < chain.spaceNum(); ++frameId) {
				skelTree::Matrix44 mat = chain.restMatrix(frameId);
				for (auto& p : p4) {
					p1 = p * mat;
					mat.multVecMatrix(p, p1);
					vertices[idx++] = p1.x; vertices[idx++] = p1.y; vertices[idx++] = p1.z;
				}
			}
		}
	}

	void _fillIndex(unsigned int* indices)
	{
		uint idx = 0, frameBeginId = 0;
		uint idOffset[6] = { 0,1,0,2,0,3 };
		for (uint i = 0; i < skTree.chainNum(); ++i) {
			skelTree::CRChain chain = skTree.getChain(i);
			for (uint frameId = 0; frameId < chain.spaceNum(); ++frameId, frameBeginId+=4) {
				for (auto pId : idOffset)
					indices[idx++] = frameBeginId + pId;
			}
		}
	}
};