#pragma once

#include <SkelTreeBase.h>

NS_BEGIN

enum AttrType {Rest, Deformed};

STRUCT(	Points,
		VecListList attrList;
		Points() { attrList.push_back(VecList()); };
		RVecList rest() { return attrList[AttrType::Rest]; };
		RVecList deformed() { return attrList[AttrType::Deformed]; };
		void addDeformed() { if (attrList.size() < 2) attrList.push_back(rest()); };
		CRVecList finalPositions() const { return (attrList.size() < 2) ? attrList[AttrType::Rest] : attrList[AttrType::Deformed]; };
		Uint pointNum() const { return Uint(attrList[AttrType::Rest].size()); };
		void clear() { for (auto& attr : attrList) attr.clear(); };
		)

NS_END