#include "Light.h"

namespace SRay
{

	Light::Light(char *name, fcolor pColor, _float pCoef)
		:m_color(pColor), Object(name), m_coef(pCoef)
	{
		_type = LIGHT;
	}

	Light::~Light(void)
	{
	}

}
