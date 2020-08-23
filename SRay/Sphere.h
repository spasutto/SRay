#pragma once

#include "Common.h"
#include "Shape.h"

#include "../Utility/Utility.h" // TODO: virer ce include

namespace SRay
{

	class Sphere : public Shape
	{
	public:
		Sphere(char *name, Material *pMaterial = new Material(), position pOrigin = position(200.0f, 0, 0), double pRadius = 25.0f)
			:m_origin(pOrigin), Shape(name, pMaterial)
		{
			_type = SPHERE;
			setRadius(pRadius);
		};
		Sphere(char *name = NULL, fcolor pColor = fblanc, position pOrigin = position(200.0f, 0, 0), double pRadius = 25.0f) :m_origin(pOrigin), Shape(name, pColor)
		{
			_type = SPHERE;
			setRadius(pRadius);
		};
		~Sphere(void);

		virtual bool Intersect(Ray &pRay, double *pDist = NULL, position *pInter = NULL, bool *pSortant = NULL) //override C++11 seulement...
		{
			Vector L;
			double tca, thc;
			L = Vector(m_origin.x - pRay.m_origin.x, m_origin.y - pRay.m_origin.y, m_origin.z - pRay.m_origin.z); // m_origin - pRay.m_origin;
			tca = L.dotProduct(pRay.m_direction);
			if (tca < 0.0f)
				return false;
			thc = L.Length();
			thc = thc*thc - tca*tca;
			if (/*thc < 0.0f || */(thc > m_radius2))
				return false;
			thc = m_radius2 - thc;
			if (thc < 0.0f)
				return false;
			thc = sqrt(thc);
			// solutions: tca+thc et tca-thc. On ne garde donc que la plus petite positive
			if ((tca - thc) < EPSILON)
			{
				if (pDist) *pDist = tca + thc;
				if (pSortant)*pSortant = true;
			}
			else
			{
				if (pDist) *pDist = tca - thc;
				if (pSortant)*pSortant = false;
			}
			//Utility::debug_log("\n###pdist=%f###", pDist);
			//printf("tca=%f thc=%f pDist=%f ", tca, thc, pDist);
			if (pInter)
			{
				pInter->x = pRay.m_origin.x + pRay.m_direction.x**pDist;
				pInter->y = pRay.m_origin.y + pRay.m_direction.y**pDist;
				pInter->z = pRay.m_origin.z + pRay.m_direction.z**pDist;
				m_normal = Vector(*pInter) - m_origin;
				//printf("ext %f", m_normal.Length());
				m_normal.Normalize();
				if (pSortant && *pSortant) // le point d'observation est dans la sphère
					m_normal = -m_normal;
			}
			//printf("\n\n");
			return true;
		}

		virtual void Normal(position& pos, Vector& norm)
		{
			norm = m_normal;
			return;
			norm.x = (pos.x - m_origin.x);
			norm.y = (pos.y - m_origin.y);
			norm.z = (pos.z - m_origin.z);
			norm /= m_radius;
		}

		virtual void setRadius(double &pRadius)
		{
			m_radius = pRadius;
			m_radius2 = pRadius*pRadius;
		}

		virtual void setCenter(position pCenter)
		{
			m_origin = pCenter;
		}

		position	m_origin;
		double		m_radius, m_radius2;
	};

}
