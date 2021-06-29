#pragma once

#include <PopulateGeometryBase.h>

class PopulateGeometryChains : public PopulateGeometryBase
{
public:
	void prepare(skelTree::SkelTreeDataP pTreeData, const uint vertexOffsetId, const uint indexOffsetId) override
	{
		PopulateGeometryBase::prepare(pTreeData, vertexOffsetId, indexOffsetId);
		skTree.reset(pTreeData);
		skTree.buildChains();
	}
	uint vertexSize() const
	{
		return  skTree.spaceNum() * 4;
	}
	uint indexSize() const
	{
		return  skTree.spaceNum() * 6;
	}
	void populateGeometryPosition(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		uint idx = _positionOffsetId();
		skelTree::Vec p4[4] = {
			skelTree::Vec(0, 0, 0),
			skelTree::Vec(1, 0, 0),
			skelTree::Vec(0, 1, 0),
			skelTree::Vec(0, 0, 1)
		}, p1;

		for (uint i = 0; i < skTree.chainNum(); ++i) {
			skelTree::CRChain chain = skTree.getChain(i);
			for (uint frameId = 0; frameId < chain.spaceNum(); ++frameId) {
				skelTree::Matrix44 mat = chain.matrix(frameId);
				for (auto& p : p4) {
					p1 = p * mat;
					mat.multVecMatrix(p, p1);
					buf[idx++] = p1.x; buf[idx++] = p1.y; buf[idx++] = p1.z;
				}
			}
		}
	}
	void populateGeometryColor(MGeometry& data, MVertexBufferDescriptor& vertexBufferDescriptor, float* buf) override
	{
		skelTree::Vec c4[4] = {
			skelTree::Vec(0, 0, 0),
			skelTree::Vec(1, 0, 0),
			skelTree::Vec(0, 1, 0),
			skelTree::Vec(0, 0, 1)
		};

		uint idx = _colorOffsetId();
		for (uint i = 0; i < skTree.spaceNum(); ++i) {
			for (uint i4 = 0; i4 < 4; ++i4) {
				buf[idx++] = c4[i4].x;
				buf[idx++] = c4[i4].y;
				buf[idx++] = c4[i4].z;
				buf[idx++] = 1.0f;
			}
		}
	}

	void populateGeometryIndex(MGeometry& data, const MRenderItem* item, const MString& renderItemName) override
	{
		uint indexSze = indexSize();
		MIndexBuffer* indexBuffer = data.createIndexBuffer(MGeometry::kUnsignedInt32);

		if (item->name() == renderItemName)
		{
			unsigned int* indices = (unsigned int*)indexBuffer->acquire(indexSze, true);
			_fillIndex(indices);
			indexBuffer->commit(indices);
		}
		item->associateWithIndexBuffer(indexBuffer);
	}

private:
	skelTree::SkelTree skTree;

	void _fillIndex(unsigned int* indices)
	{
		uint idx = 0, frameBeginId = _indexOffsetId;
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