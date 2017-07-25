#pragma once

#include "Common.h"

namespace SRay
{

	enum TYPE_OBJECT
	{
		OBJECT, SHAPE, LIGHT, MATERIAL, CUBE, CYLINDER, MESH, PLAN, POINTLIGHT, AREALIGHT, SPHERE, TRIANGLE
	};

class Object
{
public:
	Object(const char *name = NULL);
	virtual ~Object(void);
	void setName(const char *name);
	virtual TYPE_OBJECT getType() { return _type; }

	char		m_name[32];
	//const std::type_info *mytype = &typeid(*this);
protected:
	TYPE_OBJECT _type = OBJECT;
};

}
