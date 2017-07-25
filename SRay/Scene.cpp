#include "Scene.h"

namespace SRay
{

Scene::Scene(void)
{
}

Scene::~Scene(void)
{
	clearScene();
}

void Scene::clearScene(void)
{
	while (!m_shapes.empty())
	{
		delete m_shapes.back();
		m_shapes.pop_back();
	}
	while (!m_lights.empty())
	{
		delete m_lights.back();
		m_lights.pop_back();
	}
	while (!m_materials.empty())
	{
		delete m_materials.back();
		m_materials.pop_back();
	}
}

}
