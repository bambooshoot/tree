#pragma once

#include <SkelTreeBase.h>

NS_BEGIN

#define ATTRTYPE enum AttrType { Rest = 0, Deformed = 1 };

STRUCT(	Points,
		ATTRTYPE
		VecListList attrList;
		Points() { attrList.push_back(VecList()); };
		RVecList rest() { return attrList[AttrType::Rest]; };
		RVecList deformed() { return attrList[AttrType::Deformed]; };
		void addDeformed() { if (attrList.size() < 2) attrList.push_back(rest()); };
		CRVecList finalPositions() const { return (attrList.size() < 2) ? attrList[AttrType::Rest] : attrList[AttrType::Deformed]; };
		Uint pointNum() const { return Uint(attrList[AttrType::Rest].size()); };
		void clear() { for (auto& attr : attrList) attr.clear(); };
		)

#undef ATTRTYPE

NS_END