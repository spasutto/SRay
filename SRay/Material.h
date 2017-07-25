#pragma once

#include "Common.h"
#include "Object.h"

namespace SRay
{

	typedef double(*pos_func)(double x, double y, double z);

class Material : public Object
{
public:
	Material(void);
	~Material(void);
	void setRed(uchar r) { m_color.r = float(r) / 255.0f; }
	void setGreen(uchar g) { m_color.g = float(g) / 255.0f; }
	void setBlue(uchar b) { m_color.b = float(b) / 255.0f; }
	void setColor(fcolor &pColor) {m_color = pColor;}
	void setColor(uchar r, uchar g, uchar b) { m_color.r = float(r) / 255.0f; m_color.g = float(g) / 255.0f; m_color.b = float(b) / 255.0f; }
	void setkSpec(_float kspec) { kSpecular = kspec; /*kDiffus = 1 - kspec;*/ }
	void setkDif(_float kdif) { kDiffus = kdif;  /*kSpecular = 1 - kdif;*/ }
	void setkShin(ushort kshin) { kShin = kshin; }
	void setkReflec(_float kreflec) { kReflec = kreflec;  kRefrac = 1 - kreflec; }
	void setkRefrac(_float krefrac) { kRefrac = krefrac;  kReflec = 1 - krefrac; }
	void setIndice(_float ind) { nIndice = ind; }
	pos_func displacement_func = NULL;

	fcolor			m_color;
	_float		kSpecular;
	ushort		kShin;
	_float		kDiffus;
	_float		kReflec;
	_float		kRefrac;
	_float		nIndice;
};

}
