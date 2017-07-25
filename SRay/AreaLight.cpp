#include "AreaLight.h"

namespace SRay
{

	AreaLight::AreaLight(char *name, position pOrigin, float psize, Vector pnorm, fcolor pColor, _float pCoef)
		:Light(name, pColor, pCoef), origin(pOrigin), size(psize), normal(pnorm)
	{
		_type = AREALIGHT;
	}

	AreaLight::~AreaLight(void)
	{
	}

}
