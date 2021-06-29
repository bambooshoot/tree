#pragma once

#include <SkelTreeBase.h>

NS_BEGIN

CLASS(Space,
public:
	virtual Matrix44 matrix(CRQuat q) const = 0;
	virtual Matrix44 restMatrix() const = 0;
	virtual Float xParam() const = 0;
	)

NS_END