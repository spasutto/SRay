#pragma once

#include "Common.h"
#include "Object.h"
#include "Ray.h"
#include "Material.h"

namespace SRay
{

class Shape : public Object
{
public:
	Shape(char *name, Material *pMaterial);
	Shape(char *name, fcolor &pColor);
	virtual ~Shape(void);
	void setColor(fcolor &pColor);
	void setColor(uchar r, uchar g, uchar b);
	void setMaterial(Material *pMaterial);

	virtual bool Intersect(Ray &pRay, double *pDist = NULL, position *pInter = NULL, bool *pSortant = NULL)
	{
		return false;
	}

	virtual void Normal(position&, Vector&){}

	Material	*m_material;
	Vector m_normal;
private:
	bool	bSelfMaterial;
};

}
