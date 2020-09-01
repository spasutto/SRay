#pragma once

#include "Common.h"
#include "Viewplane.h"

namespace SRay
{
class Camera
{
public:
	Camera(ushort pWidth = 800, ushort pHeight = 600, double pFov = DEFAULT_FOV);
	~Camera(void);

	void setHFov(double phfov) noexcept { m_hfov = phfov; }
	double getHFov() noexcept { return m_hfov; }
	void setVFov(double pvfov) noexcept { m_vfov = pvfov; }
	double getVFov() noexcept { return m_vfov; }
	void setFov(double pfov) noexcept { setHFov(pfov); setVFov(pfov); }
	INLINE Vector getDirection() noexcept { return vFront; }
	INLINE position getDir() noexcept { return m_dir; }
	INLINE position getPos() noexcept { return m_pos; }
	void setPos(double x, double y, double z) { setPos(position(x, y, z)); }
	void setPos(position &pos);

	void setDirection(Vector dir);
	void setDirection(position &dir);
	void setDirection(double x, double y, double z);// { setDirection(position(x, y, z)); }
	int getWidthVP() noexcept { return m_viewplane.m_width; }
	int getHeightVP() noexcept { return m_viewplane.m_height; }
	void setWidthVP(ushort pWidth){ m_viewplane.setWidth(pWidth); }
	void setHeightVP(ushort pHeight){ m_viewplane.setHeight(pHeight); }
	double getXIndentVP() noexcept { return m_viewplane.m_xindent; }
	double getYIndentVP() noexcept { return m_viewplane.m_yindent; }
	void init();
protected:
	void setDirectionInternal(Vector &dir);
	void setDirectionNormal(Vector &dir);
public:
	double		m_distvp;	// distance au centre du VP
	Vector		vRight, vUp, vFront; // repère de la caméra (vFront == direction de vue)
	Vector		m_init; //vecteur pointant vers le premier "pixel" du viewplane
protected:
	Viewplane	m_viewplane;
	position	m_pos;
	position m_dir;
	double		m_hfov, m_vfov;
private:
	bool bDirPos; // si la caméra vise une POSITION (et non pas une direction)
};

}
