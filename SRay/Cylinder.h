#pragma once

#include "Common.h"
#include "Shape.h"

namespace SRay
{

class Cylinder : public Shape
{
public:
	Cylinder(char *name, Material *pMaterial = new Material(), position pOrigin=position(200.0f, 0, 0), double pHeight=50.0f, double pRayon=25.0f):m_origin(pOrigin),m_height(pHeight),m_rayon(pRayon),Shape(name, pMaterial) {};
	Cylinder(char *name = NULL, fcolor pColor = fblanc, position pOrigin=position(200.0f, 0, 0), double pHeight=50.0f, double pRayon=25.0f):m_origin(pOrigin),m_height(pHeight),m_rayon(pRayon),Shape(name, pColor) {};
	~Cylinder(void);

	virtual bool Intersect(Ray &pRay, double *pDist, position *pInter = NULL, bool *pSortant = NULL)
	{
		return false;
	}

	virtual void Normal(position& pos, Vector& norm)
	{
	}

	position	m_origin;
	double		m_height;
	double		m_rayon;
};

}
