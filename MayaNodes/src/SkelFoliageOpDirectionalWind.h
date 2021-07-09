#include <SkelFoliageOpBase.h>

NS_BEGIN

class FoliageOpDirectionalWind : public FoliageOpBase
{
public:
	Quat operator()(CRFoliageData foliageData, CRFoliageOpData opData) override
	{
		Quat q;
		return q;
	}
};

NS_END