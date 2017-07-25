#pragma once

#include "Common.h"
#include "Shape.h"

namespace SRay
{

class Cube : public Shape
{
public:
	Cube(char *name = NULL, fcolor pColor = fblanc, Vector pB1 = Vector(-25.0f, -25.0f, 0), Vector pB2 = Vector(25.0f, 25.0f, 0))
		:Shape(name, pColor)
	{
		bounds[0] = pB1;
		bounds[1] = pB2;
	}
	~Cube(void);

	virtual bool Intersect(Ray &pRay, double *pDist, position *pInter = NULL, bool *pSortant = NULL)
	{
		double tmin, tmax, tymin, tymax, tzmin, tzmax;
		if (pRay.m_direction.x >= 0)
		{
			tmin = (bounds[0].x - pRay.m_origin.x) / pRay.m_direction.x;
			tmax = (bounds[1].x - pRay.m_origin.x) / pRay.m_direction.x;
		}
		else
		{
			tmin = (bounds[1].x - pRay.m_origin.x) / pRay.m_direction.x;
			tmax = (bounds[0].x - pRay.m_origin.x) / pRay.m_direction.x;
		}
		if (pRay.m_direction.y >= 0)
		{
			tymin = (bounds[0].y - pRay.m_origin.y) / pRay.m_direction.y;
			tymax = (bounds[1].y - pRay.m_origin.y) / pRay.m_direction.y;
		}
		else
		{
			tymin = (bounds[1].y - pRay.m_origin.y) / pRay.m_direction.y;
			tymax = (bounds[0].y - pRay.m_origin.y) / pRay.m_direction.y;
		}
		if ( (tmin > tymax) || (tymin > tmax) )
			return false;
		if (tymin > tmin)
			tmin = tymin;
		if (tymax < tmax)
			tmax = tymax;
		if (pRay.m_direction.z >= 0)
		{
			tzmin = (bounds[0].z - pRay.m_origin.z) / pRay.m_direction.z;
			tzmax = (bounds[1].z - pRay.m_origin.z) / pRay.m_direction.z;
		}
		else
		{
			tzmin = (bounds[1].z - pRay.m_origin.z) / pRay.m_direction.z;
			tzmax = (bounds[0].z - pRay.m_origin.z) / pRay.m_direction.z;
		}
		if ( (tmin > tzmax) || (tzmin > tmax) )
			return false;
		/*if (tzmin > tmin)
			tmin = tzmin;
		if (tzmax < tmax)
			tmax = tzmax;*/
		if (pInter)
		{
			pInter->x = tmin;
			pInter->y = tmax;
			pInter->z = bounds[0].z;
		}
		return true;//( (tmin < t1) && (tmax > t0) );
	}

	virtual void Normal(position& pos, Vector& norm)
	{
		if (fabs(bounds[0].x - pos.x) < EPSILON)
			norm = Vector(-1, 0, 0);
		else if (fabs(bounds[1].x - pos.x) < EPSILON)
			norm = Vector(1, 0, 0);
		else if (fabs(bounds[0].y - pos.y) < EPSILON)
			norm = Vector(0, -1, 0);
		else if (fabs(bounds[1].y - pos.y) < EPSILON)
			norm = Vector(0, 1, 0);
		else if (fabs(bounds[0].z - pos.z) < EPSILON)
			norm = Vector(0, 0, -1);
		else
			norm = Vector(0, 0, 1);
	}

	Vector		bounds[2];
};

}
