
#pragma once

#include "Common.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"

#include "Sphere.h"
#include "Plan.h"
#include "Cube.h"
#include "Triangle.h"
#include "Mesh.h"
#include "PointLight.h"
#include "AreaLight.h"

namespace SRay
{

class RayTracer
{
public:
	RayTracer(uint pWidth=0, uint pHeight=0);
	~RayTracer(void);
	void init();
	void initPixelBuffer();
	INLINE void freePixelBuffer();
	int Render(void);

	void setScene(Scene *pScene) {m_scene = *pScene;}
	Scene *getScene() {return &m_scene;}
	void clearScene(bool bClearObj = true) { if (!bAmbient) memset(&ambient, (uchar)(kAmbient*255.0f), sizeof(color)); memset(m_image, (uchar)(kAmbient*255.0f), getWidth()*getHeight()*sizeof(color)); if (bClearObj) m_scene.clearScene(); }
	void setCamera(Camera *pcamera) {m_camera = *pcamera;}
	Camera *getCamera() {return &m_camera;}

	INLINE ushort getWidth() {return m_camera.getWidthVP();}
	INLINE ushort getHeight() {return m_camera.getHeightVP();}
	INLINE void setWidth(ushort pWidth) { bInitOK = false; m_camera.setWidthVP(pWidth); }
	INLINE void setHeight(ushort pHeight) { bInitOK = false; m_camera.setHeightVP(pHeight); }

	void setNRec(ushort pNRec) {nrec = pNRec;}
	void setkAmbient(_float kambient)
	{
		bKAmbient = true;
		kAmbient = kambient;
		if (!bAmbient)
			ambient = fcolor{ kAmbient, kAmbient, kAmbient };
	}
	void setAmbient(uchar r, uchar g, uchar b) { bAmbient = true; ambient.r = r; ambient.g = g; ambient.b = b; }
	void setAALevel(ushort level) { if (aa > 0) aa = level; }

protected:
	fcolor launchRay(Ray &pRay, uchar level = 0, double nprec = 1.0f);

public:
//protected:
	color *m_image = NULL;

protected:
	Scene		m_scene;
	Camera	m_camera;
	ushort	nrec;
	color		ambient;
	ushort	aa;
public:
	_float	   kAmbient;
private:
// variables de travail:
	bool bKAmbient = false;
	bool bAmbient = false;
	bool bInitOK = false;
};

}
