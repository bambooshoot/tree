#include <DrawableBufferBase.h>

std::map<MGeometry::Semantic, MString> DrawableBufferBase::sSematicName = {
			{MGeometry::kPosition, MString("positions")},
			{MGeometry::kColor, MString("colors") },
			{MGeometry::kNormal, MString("normals") }
};