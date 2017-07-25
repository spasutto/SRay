#pragma once

#include "Common.h"
#include "Light.h"

namespace SRay
{

	class PointLight : public Light
{
public:
	PointLight(char *name = NULL, position pOrigin = position(0, 0, -50.0f), fcolor pColor = fblanc, _float pCoef = 1.0f);
	~PointLight(void);
	void setPos(double x, double y, double z){m_origin = position(x,y,z);}
	void setPos(position &pos){m_origin = pos;}

	virtual float CalcVisibility(position *intersect, Ray *lightray, double *objDist, ObjVector<Shape*> *shapes, Shape *shape)
	{
		uint i;
		Shape *pShape;
		double objDisttemp;
		lightray->m_origin = this->m_origin;
		lightray->m_direction = *intersect;
		lightray->m_direction -= lightray->m_origin;
		*objDist = lightray->m_direction.Length();
		lightray->m_direction.Normalize();

		for (i = 0; i<shapes->size(); i++)
		{
			pShape = (*shapes)[i];
			if (pShape == shape) continue;
			if (pShape->Intersect(*lightray, &objDisttemp))
			{
				if ((objDisttemp - *objDist) < 0.0f) // WARNING : <=
				{
					return 0.0f;
					break;
				}
			}
		}

		return 1.0f;
	}

	position	m_origin;
};

}
