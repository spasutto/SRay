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

	void setHFov(double phfov) { m_hfov = phfov; }
	double getHFov() { return m_hfov; }
	void setVFov(double pvfov) { m_vfov = pvfov; }
	double getVFov() { return m_vfov; }
	void setFov(double pfov) { setHFov(pfov); setVFov(pfov); }
	INLINE position getPos() { return m_pos; }
	void setPos(double x, double y, double z) { setPos(position(x, y, z)); }
	void setPos(position &pos);
	
	void setDirection(Vector dir);
	void setDirection(position &dir);
	void setDirection(double x, double y, double z);// { setDirection(position(x, y, z)); }
	int getWidthVP() { return m_viewplane.m_width; }
	int getHeightVP() { return m_viewplane.m_height; }
	void setWidthVP(ushort pWidth){ m_viewplane.setWidth(pWidth); }
	void setHeightVP(ushort pHeight){ m_viewplane.setHeight(pHeight); }
	double getXIndentVP(){ return m_viewplane.m_xindent; }
	double getYIndentVP(){ return m_viewplane.m_yindent; }
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
