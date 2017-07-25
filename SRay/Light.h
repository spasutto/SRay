#pragma once

#include "Common.h"
#include "Object.h"
#include "Ray.h"
#include "Shape.h"

namespace SRay
{

class Light : public Object
{
public:
	Light(char *name = NULL, fcolor pColor = fblanc, _float pCoef = 1.0f);
	~Light(void);
	void setPower(_float pCoef){ m_coef = pCoef; }
	void setRed(uchar r) { m_color.r = float(r) / 255.0f; }
	void setGreen(uchar g) { m_color.g = float(g) / 255.0f; }
	void setBlue(uchar b) { m_color.b = float(b) / 255.0f; }
	void setColor(fcolor &pColor) { m_color = pColor; }
	void setColor(uchar r, uchar g, uchar b) { m_color.r = float(r) / 255.0f; m_color.g = float(g) / 255.0f; m_color.b = float(b) / 255.0f; }

	virtual float CalcVisibility(position *intersect, Ray *lightray, double *objDist, ObjVector<Shape*> *shapes, Shape *shape) = 0;

	fcolor		m_color;
	_float		m_coef;
};

}
