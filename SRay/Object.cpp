#include "Object.h"

namespace SRay
{

Object::Object(const char *name)
{
	_type = OBJECT;
	setName(name);
}

Object::~Object(void)
{
}

void Object::setName(const char *name)
{
	if (name != NULL)
	{
		strncpy(m_name, name, sizeof(m_name)-1);
		m_name[MIN(strlen(name), sizeof(m_name)-1)] = 0;
	}
	else
		m_name[0]=0;
}

}
