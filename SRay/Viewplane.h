#pragma once

#include "Common.h"

namespace SRay
{

class Viewplane
{
public:
	Viewplane(double pWidthr=133, double pHeightr=100, ushort pWidth=800, ushort pHeight=600);
	Viewplane(ushort pWidth, ushort pHeight);
	~Viewplane(void);

	void setWidth(ushort pWidth){ m_width = pWidth; }
	void setHeight(ushort pHeight){ m_height = pHeight; }
	void init(bool bCalcRDim = true);
protected:
public:
	ushort		m_width, m_height;//virtuel (800, 600)
	double		m_widthr, m_heightr;//réel (311, 212)
	double		m_xindent, m_yindent;
};

}
