#pragma once
#include <ai.h>
#include <sstream>
#include <random>

#include <SkelTreeData.h>
#include <SkelTree.h>

struct SkelTreeOpData
{
	AtString trunkMesh;
	AtString foliageMesh;
	AtString skelTreeFile;
	int      seed;
};

using namespace skelTree;

class SkelTreeArBuilder
{
public:
	void build(const SkelTreeOpData* skelTreeOpData);

private:
	AtNode* nodeLookUpByName(const AtString& name);
	void fillTreeMesh(SkelTreeDataP pTreeData, AtNode* atMeshNode);
	void setTrunkMeshPoints(SkelTreeDataP pTreeData, AtNode* atMeshNode);
	void genFolidages(SkelTree & tree, AtNode* atMeshNode);
	float getFrame();
};