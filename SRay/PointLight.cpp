#include "PointLight.h"

namespace SRay
{

	PointLight::PointLight(char *name, position pOrigin, fcolor pColor, _float pCoef)
		:Light(name, pColor, pCoef)
	{
		_type = POINTLIGHT;
	}

	PointLight::~PointLight(void)
	{
	}

}
