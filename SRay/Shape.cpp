#include "Shape.h"

namespace SRay
{

Shape::Shape(char *name, Material *pMaterial)
:m_material(pMaterial), Object(name), bSelfMaterial(false)
{
	_type = SHAPE;
}

Shape::Shape(char *name, fcolor &pColor)
: Object(name), m_material(NULL), bSelfMaterial(true)
{
	_type = SHAPE;
	setColor(pColor);
}

Shape::~Shape(void)
{
	if (bSelfMaterial && m_material != NULL)
		delete m_material;
}

void Shape::setColor(fcolor &pColor)
{
	if (bSelfMaterial && m_material != NULL)
		delete m_material;

	bSelfMaterial = true;
	m_material = new Material();
	m_material->m_color = pColor;
}

void Shape::setColor(uchar r, uchar g, uchar b)
{
	if (m_material != NULL)
		delete m_material;

	bSelfMaterial = true;
	m_material = new Material();
	fcolor tmpcolor = { float(r) / 255.0f, float(g) / 255.0f, float(b) / 255.0f };
	m_material->m_color = tmpcolor;
}

void Shape::setMaterial(Material *pMaterial)
{
	if (bSelfMaterial && m_material != NULL)
		delete m_material;

	m_material = pMaterial;
	bSelfMaterial = false;
}

}
