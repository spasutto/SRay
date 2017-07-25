#pragma once

#include "Common.h"

namespace SRay
{

class Ray
{
public:
	Ray(void);
	~Ray(void);

//protected:
	position m_origin;
	Vector m_direction;
};

}
