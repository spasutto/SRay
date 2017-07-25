#pragma once

#include "Common.h"
#include "Shape.h"
#include "Light.h"

namespace SRay
{
class Scene
{
public:
	Scene(void);
	~Scene(void);
	void Add(Shape* obj) { m_shapes.push_back(obj); }
	void Add(Light* light) { m_lights.push_back(light); }
	void Add(Material* mat) { m_materials.push_back(mat); }
	void clearScene(void);

	Shape *operator[](const uint pos) { return m_shapes.at(pos); }

	ObjVector<Shape*>			m_shapes;
	ObjVector<Light*>			m_lights;
	ObjVector<Material*>	m_materials;
};

}
