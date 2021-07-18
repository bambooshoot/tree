#include <SkelTreeArBuilder.h>
#include <SkelTreeFile.h>
#include <SkelTree.h>

AtNode* SkelTreeArBuilder::nodeLookUpByName(const AtString& name)
{
	std::string nodeName(name);
	if (nodeName.empty())
		return nullptr;

	AtNode* pNode;
	if (AI_VERSION_ARCH_NUM == 6) {
		if (nodeName.at(0) != '|' && nodeName.find_first_of('|') < nodeName.size())
			nodeName = nodeName.insert(0, "|");
		else if (nodeName.at(0) != '/' && nodeName.find_first_of('/') < nodeName.size())
			nodeName = nodeName.insert(0, "/");

		pNode = AiNodeLookUpByName(AtString(nodeName.c_str()));

		if (pNode)
			return pNode;

		std::replace(nodeName.begin(), nodeName.end(), '|', '/');
	}

	pNode = AiNodeLookUpByName(AtString(nodeName.c_str()));
	return pNode;
}

using namespace skelTree;

void SkelTreeArBuilder::fillTreeMesh(skelTree::SkelTreeDataP pTreeData, AtNode* atMeshNode)
{
	AtArray* pointArray = AiNodeGetArray(atMeshNode, "vlist");
	AtVector* pList = (AtVector*)AiArrayMapKey(pointArray, 0);
	Uint pointNum = (Uint)AiArrayGetNumElements(pointArray);
	RPoints points = pTreeData->addPoints();
	RVecList posList = points.rest();
	posList.resize(pointNum);
	for (Uint i = 0; i < pointNum; ++i) {
		posList[i].setValue(pList[i].x, pList[i].y, pList[i].z);
	}
}

void SkelTreeArBuilder::setTrunkMeshPoints(SkelTreeDataP pTreeData, AtNode* atTrunkMeshNode)
{
	CRVecList posList = pTreeData->pointsList[0].finalPositions();
	AtArray* point_array = AiArrayAllocate((Uint)posList.size(), 1, AI_TYPE_VECTOR);
	AtVector* pList = (AtVector*)AiArrayMapKey(point_array, 0);
	for (Uint i = 0; i < posList.size();++i) {
		pList[i].x = pList[i].x;
		pList[i].y = pList[i].y;
		pList[i].z = pList[i].z;
	}
	AiArrayUnmap(point_array);
	AiNodeSetArray(atTrunkMeshNode, "vlist", point_array);
}

void SkelTreeArBuilder::genFolidages(SkelTree& tree, AtNode* atMeshNode)
{
	const char* srcMeshName = AiNodeGetName(atMeshNode);
	std::string instanceBaseName(srcMeshName);
	instanceBaseName += "/instance";
	std::string instanceName;
	
	for (Uint i = 0; i < tree.foliageNum(); ++i) {
		instanceName = instanceBaseName;
		instanceName += i;
		AtNode *instanceNode = AiNode("ginstance", AtString(instanceName.c_str()));
		AtMatrix matrix;
		CRMatrix44 mat44 = tree.getFoliageMatrix(i);
		MatrixTranslate<CMatrix44, AtMatrix, 4>(mat44, matrix);
		AiNodeSetMatrix(instanceNode, "matrix", matrix);
		AiNodeSetPtr(instanceNode, "node", atMeshNode);
	}
}

float SkelTreeArBuilder::getFrame()
{
	AtNode* optNode = AiNode("options");
	return AiNodeGetFlt(optNode, "frame");
}

void SkelTreeArBuilder::build(const SkelTreeOpData* skelTreeOpData)
{
	AtNode* trunkMeshNode = nodeLookUpByName(skelTreeOpData->trunkMesh);
	AtNode* foliageMeshNode = nodeLookUpByName(skelTreeOpData->foliageMesh);

	if (!trunkMeshNode) {
		AiMsgError("No node has been found as trunk. : %s", skelTreeOpData->trunkMesh);
		return;
	}

	if (!foliageMeshNode) {
		AiMsgError("No node has been found as foliage. : %s", skelTreeOpData->foliageMesh);
		return;
	}

	SkelTreeData treeData;
	AniOpData opData;
	skelTree::File::read(treeData, opData, skelTreeOpData->skelTreeFile);

	fillTreeMesh(&treeData, trunkMeshNode);
	fillTreeMesh(&treeData, foliageMeshNode);

	opData.time = getFrame();

	SkelTree tree;
	tree.buildStruct(&treeData);
	tree.deformAndFoliages(&treeData, opData);

	setTrunkMeshPoints(&treeData, trunkMeshNode);
	genFolidages(tree, foliageMeshNode);
}
