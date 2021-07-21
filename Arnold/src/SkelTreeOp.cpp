#include <ai.h>
#include <sstream>
#include <random>

#include <SkelTreeArBuilder.h>

AI_OPERATOR_NODE_EXPORT_METHODS(OpMethods);

namespace
{
	std::uniform_real_distribution<float> dist(0.0, 1.0);
	std::mt19937 rand_generator;

	float random_value()
	{
		return dist(rand_generator);
	}
}

node_parameters
{
	// scatter params
	AiParameterStr("trunkMesh", "");
	AiParameterStr("foliageMesh", "");
	AiParameterStr("skelTreeFile", "");
	AiParameterFlt("offset", 0);
}

operator_init
{
	SkelTreeOpData* pData = new SkelTreeOpData();
	*user_data = pData;

	pData->trunkMesh = AiNodeGetStr(op, "trunkMesh");
	pData->foliageMesh = AiNodeGetStr(op, "foliageMesh");
	pData->skelTreeFile = AiNodeGetStr(op, "skelTreeFile");
	pData->offset = AiNodeGetFlt(op, "offset");

	return true;
}

operator_cleanup
{
	return true;
}

bool cleanup_child_data(void* child_data)
{
	delete static_cast<SkelTreeOpData*>(child_data);
	return true;
}

operator_cook
{
	SkelTreeOpData * pData = static_cast<SkelTreeOpData*>(user_data);
	
	SkelTreeArBuilder builder;
	builder.build(pData);

	return true;
}

operator_post_cook
{
	return true;
}