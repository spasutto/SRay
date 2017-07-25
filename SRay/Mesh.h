#pragma once

#include "Common.h"
#include "Shape.h"
#include "Triangle.h"
#include "../Utility/Utility.h" // TODO: virer ce include
#include "../Parser/ObjFile.h"

#include <vector>

using namespace std;
using namespace SRay::Parser;

namespace SRay
{

class Mesh : public Shape
{
public:
	Mesh(char *name, Material *pMaterial = new Material()):Shape(name, pMaterial) {}
	Mesh(char *name = NULL, fcolor pColor = fblanc):Shape(name, pColor) {}
	~Mesh(void);

	virtual bool Intersect(Ray &pRay, double *pDist, position *pInter = NULL, bool *pSortant = NULL) //override C++11 seulement...
	{
		for (uint i=0; i<triangles.size(); i++)
		{
			if (triangles[i]->Intersect(pRay, pDist, pInter, pSortant))
			{
				triangles[i]->Normal(*pInter, m_normal);
				return true;
			}
		}
		return false;
	}

	virtual void Normal(position& pos, Vector& norm)
	{
		norm = m_normal;
		return ;
	}

	void Load(char *filename)
	{
		ObjFile ofile(filename, false);
		ofile.Load();
		for (uint i=0 ; i<ofile.faces.size(); i++)
			triangles.push_back(ofile.faces[i]);
	}

	position	m_origin;
	Vector m_normal;
	vector<Triangle*> triangles;
};

}
