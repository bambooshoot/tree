#include <ai.h>

extern const AtNodeMethods* OpMethods;

node_loader
{
	switch (i) {
	case 0:
		node->methods = OpMethods;
		node->output_type = AI_TYPE_NONE;
		node->name = AtString("SkelTreeOp");
		node->node_type = AI_NODE_OPERATOR;
		break;
	default:
		return false;
	}

	strcpy_s(node->version, AI_VERSION);
	return true;
}