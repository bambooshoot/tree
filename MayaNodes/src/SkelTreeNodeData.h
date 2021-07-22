#pragma once

#include <maya/MPxData.h>
#include <SkelTree.h>

#include <maya/MTypeId.h>
#include <maya/MString.h>

class SkelTreeData : public MPxData
{
public:
	static void* creator();
	SkelTreeData() : pSkelTreeData(nullptr) {};
	virtual ~SkelTreeData() {}
	virtual MStatus			readASCII(const MArgList& argList,
		unsigned int& endOfTheLastParsedElement);
	virtual MStatus			readBinary(std::istream& in, unsigned int length);
	virtual MStatus			writeASCII(std::ostream& out);
	virtual MStatus			writeBinary(std::ostream& out);
	virtual MString name() const;
	virtual MTypeId typeId() const;
	virtual void copy(const MPxData& src);

	skelTree::SkelTreeDataP pSkelTreeData;

	static const MString typeName;
	static const MTypeId id;
};