#include <SkelTreeNodeData.h>

const MString SkelTreeData::typeName("SkelTreeData");
const MTypeId SkelTreeData::id(0x20220003);

MString SkelTreeData::name() const
{
	return SkelTreeData::typeName;
}
MTypeId SkelTreeData::typeId() const
{
	return SkelTreeData::id;
}
void SkelTreeData::copy(const MPxData& src)
{
	skelTreeData = ((const SkelTreeData*)&src)->skelTreeData;
}

void* SkelTreeData::creator()
{
	return new SkelTreeData();
}

MStatus SkelTreeData::readASCII(const MArgList& argList, unsigned int& endOfTheLastParsedElement)
{
	return MStatus::kSuccess;
}
MStatus SkelTreeData::readBinary(std::istream& in, unsigned int length)
{
	return MStatus::kSuccess;
}
MStatus SkelTreeData::writeASCII(std::ostream& out)
{
	return MStatus::kSuccess;
}
MStatus SkelTreeData::writeBinary(std::ostream& out)
{
	return MStatus::kSuccess;
}