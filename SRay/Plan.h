#pragma once

#include "Common.h"
#include "Shape.h"

namespace SRay
{

class Plan : public Shape
{
public:
	Plan(char *name, Material *pMaterial = new Material(), Vector pNormal = Vector(0, 0, 1.0f), _float pDistance = -50.0f) :m_distance(pDistance), Shape(name, pMaterial) { m_normal = pNormal; m_normal.Normalize(); };
	Plan(char *name = NULL, fcolor pColor = fblanc, Vector pNormal = Vector(0, 0, 1.0f), _float pDistance = -50.0f) :m_distance(pDistance), Shape(name, pColor) { m_normal = pNormal; m_normal.Normalize(); };
	~Plan(void);

	virtual bool Intersect(Ray &pRay, double *pDist = NULL, position *pInter = NULL, bool *pSortant = NULL) //override C++11 seulement...
	{
		double n = (m_distance-m_normal.dotProduct(pRay.m_origin));
		double d = pRay.m_direction.dotProduct(m_normal);
		if ((n >= -EPSILON && n <= EPSILON) || (d >= -EPSILON && d <= EPSILON)) // le rayon est contenu dans le plan || le rayon est parallèle au plan
			return false;
		if (((n < 0.0f) != (d < 0.0f))
			// TODO: voir si le test suivant est nécessaire, j'ai l'impression que ça fonctionne quand même quelque soit le plan et la vue?!!
			&& ((n < 0.0f) || (d < 0.0f))) // l'intersection est derrière la caméra
			return false;
		if (pDist) *pDist = n/d;
		/* Cas déjà testé avec le XOR ci dessus
		if (pDist < 0.0f)
			return false;*/
		if (pInter)
		{
			pInter->x = pRay.m_origin.x + pRay.m_direction.x**pDist;
			pInter->y = pRay.m_origin.y + pRay.m_direction.y**pDist;
			pInter->z = pRay.m_origin.z + pRay.m_direction.z**pDist;
		}
		return true;
	}

	virtual void Normal(position& pos, Vector& norm)
	{
		norm = m_normal;
	}

	virtual void setNormal(Vector pNormal)
	{
		m_normal = pNormal;
	}

	virtual void setDistance(_float pDist)
	{
		m_distance = pDist;
	}

	_float		m_distance;
};

}
