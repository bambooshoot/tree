#pragma once

#include <SkelTreeData.h>

NS_BEGIN

CLASS(Foliage,
public:
	void update(CRFoliageData foliageData, CRQuat q, CSkelTreeDataP pTreeData);
	CRMatrix44 matrix() const;

private:
	Matrix44 _matrix;
	)

NS_END