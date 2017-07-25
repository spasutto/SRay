#pragma once

#include "Common.h"
#include "Shape.h"

#define TEST_CULL

namespace SRay
{

class Triangle : public Shape
{
public:
	Triangle(char *name, Material *pMaterial = new Material())
		:Shape(name, pMaterial) { _type = TRIANGLE; }
	Triangle(char *name = NULL, fcolor pColor = fblanc)
		:Shape(name, pColor) { _type = TRIANGLE; }
	Triangle(position &pVert0, position &pVert1, position &pVert2, char *name = NULL, Material *pMaterial = new Material())
		:Shape(name, pMaterial)
	{
		_type = TRIANGLE;
		Init(pVert0, pVert1, pVert2);
	};
	Triangle(position &pVert0, position &pVert1, position &pVert2, char *name = NULL, fcolor pColor = fblanc)
		:Shape(name, pColor)
	{
		_type = TRIANGLE;
		Init(pVert0, pVert1, pVert2);
	};
	~Triangle(void);
	virtual void Init(position &pVert0, position &pVert1, position &pVert2)
	{
		vert[0] = pVert0;
		vert[1] = pVert1;
		vert[2] = pVert2;
	}

	virtual void setVert(const char pos, float xVert, float yVert, float zVert)
	{
		vert[pos] = position(xVert, yVert, zVert);
	}
	virtual void setVert(const char pos, position Vert)
	{
		vert[pos] = Vert;
	}

	virtual bool Intersect(Ray &pRay, double *pDist = NULL, position *pInter = NULL, bool *pSortant = NULL) //override C++11 seulement...
	{
		//printf("		 vert[0] : %2.3f %2.3f %2.3f (%2.3f)\n", vert[0].x, vert[0].y, vert[0].z, vert[0].Length());
		//printf("		 vert[1] : %2.3f %2.3f %2.3f (%2.3f)\n", vert[1].x, vert[1].y, vert[1].z, vert[1].Length());
		//printf("		 vert[2] : %2.3f %2.3f %2.3f (%2.3f)\n", vert[2].x, vert[2].y, vert[2].z, vert[2].Length());
		double t,u,v;
		Vector tvec, pvec, qvec, edge1, edge2;
		double det, inv_det;
		edge1 = vert[1] - vert[0];
		edge2 = vert[2] - vert[0];

		// begin calculating determinant - also used to calculate U parameter
		pvec = pRay.m_direction.crossProduct(edge2);

		// if determinant is near 0, ray lies in plane of triangle
		det = edge1.dotProduct(pvec);
		//printf("		 EDGE1 : %2.3f %2.3f %2.3f (%2.3f)\n", edge1.x, edge1.y, edge1.z, edge1.Length());
		//printf("		 EDGE2 : %2.3f %2.3f %2.3f (%2.3f)\n", edge2.x, edge2.y, edge2.z, edge2.Length());
//#define TEST_CULL
#ifdef TEST_CULL
		if (det < EPSILON)
			return false;

		tvec = pRay.m_origin - vert[0];

		u = tvec.dotProduct(pvec);
		if (u<0.0f || u>det)
			return false;

		qvec = tvec.crossProduct(edge1);

		v = pRay.m_direction.dotProduct(qvec);
		if (v<0.0f || (u+v>det))
			return false;

		t = edge2.dotProduct(qvec);
		inv_det = 1.0f/det;
		t *= inv_det;
		u *= inv_det;
		v *= inv_det;

#else
		if (det > -EPSILON && det < EPSILON)
			return false;
		inv_det = 1.0f/det;

		tvec = pRay.m_origin - vert[0];
		u = tvec.dotProduct(pvec) * inv_det;
		if (u<0.0f || u>1.0f)
			return false;

		qvec = tvec.crossProduct(edge1);
		v = pRay.m_direction.dotProduct(qvec)*inv_det;
		if (v<0.0f || (u+v>1.0f))
			return false;

		t = edge2.dotProduct(qvec)*inv_det;

#endif
		if (pDist) *pDist = t;
		if (pInter)
		{
			pInter->x = pRay.m_origin.x + pRay.m_direction.x*t;
			pInter->y = pRay.m_origin.y + pRay.m_direction.y*t;
			pInter->z = pRay.m_origin.z + pRay.m_direction.z*t;

		}
		return true;
	}

	virtual void Normal(position& pos, Vector& norm)
	{
		//AB^AC
		norm = (Vector(vert[1]) - Vector(vert[0])).crossProduct(Vector(vert[2]) - Vector(vert[0])).getNormalized();
	}

	_float		m_distance;
	Vector	vert[3];
};

}
