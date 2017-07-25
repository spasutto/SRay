#include "Viewplane.h"

namespace SRay
{

	void Viewplane::init(bool bCalcRDim)
	{
		if (m_width == 0 || m_height == 0)
			return;
		if (bCalcRDim)
		{
			m_heightr = 100.0f;
			m_widthr = m_heightr*((double)m_width / (double)m_height);
		}
		m_xindent = (double)m_widthr / m_width;
		m_yindent = (double)m_heightr / m_height;
	}

	Viewplane::Viewplane(double pWidthr, double pHeightr, ushort pWidth, ushort pHeight)
		:m_width(pWidth), m_height(pHeight), m_widthr(pWidthr), m_heightr(pHeightr)
	{
	}

	Viewplane::Viewplane(ushort pWidth, ushort pHeight)
		: m_width(pWidth), m_height(pHeight)
	{
	}

	Viewplane::~Viewplane(void)
	{
	}

}
