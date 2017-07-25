#include "Material.h"

namespace SRay
{

Material::Material(void)
:kSpecular(0.0f), kShin(50), kDiffus(1.0f), kReflec(1.0f), kRefrac(0.0f), nIndice(1.52f)
{
	m_color.r = float(rand() % 255) / 255.0f;
	m_color.g = float(rand() % 255) / 255.0f;
	m_color.b = float(rand() % 255) / 255.0f;
	_type = MATERIAL;
}

Material::~Material(void)
{
}

}
