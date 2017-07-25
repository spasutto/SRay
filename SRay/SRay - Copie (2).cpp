#include "SRay.h"

//#include <exception>	// TODO : remplacer par mécanisme d'exception
#include <cstdlib>		// TODO : supprimer dès que plus d'appels à rand()
#include <cstring>		// TODO : supprimer dès que plus d'appels à memset()

#include <new>

namespace SRay
{

void RayTracer::init()
{
	strcpy(errBuff, "Impossible d'allouer la mémoire pour le pixel buffer");
	try
	{
		m_image = new color[getWidth()*getHeight()];
	}
	catch(std::exception ex)
	{
		strcat(errBuff, ":\r\n");
		strcat(errBuff, ex.what());
		throw std::runtime_error(errBuff);
	}
	if (m_image == NULL)
		throw std::runtime_error(errBuff);
	clearScene();
}


RayTracer::RayTracer(uint pWidth, uint pHeight)
:m_camera(pWidth, pHeight),m_ambient(0.1f),nrec(10)
{
	debug_log("\n\nSRAY INIT ENGINE %dpx/%dpx\n==================================\n", pWidth, pHeight);
	init();
}

RayTracer::~RayTracer(void)
{
	if (m_image != NULL)
		delete [] m_image;
}

//TODO: replacer ces variables globales dans la fonction Render()
uint x = 0, y = 0;
int RayTracer::Render(void)
{
	Ray primaryray;
	color *curpixel = m_image;
	uint width = m_camera.getWidthVP(), height = m_camera.getHeightVP(); // compteurs de boucles
	Vector vXindent = m_camera.vRight*m_camera.getXIndentVP();
	Vector vYindent = m_camera.vUp*m_camera.getYIndentVP();

	/*primaryray.m_origin = m_camera.getPos();
	primaryray.m_direction = Vector(0,0,1);
	*curpixel = launchRay(primaryray);
	return 0;*/

	_float coeftot = 0.0f;
	std::vector<Light*>::size_type j=0;

	debug_log("SRAY RENDER %dpx/%dpx\n==================================\n", width, height);

	// normalisation des lumières
	/*for (j=0; j<m_scene.m_lights.size(); j++)
		coeftot += 2*m_scene.m_lights[j]->m_coef; // 2* pour additionner les lumières spéculaire ET diffuse
	coeftot += m_ambient;
	if (coeftot > 0.0f)
	{
		for (j = 0; j<m_scene.m_lights.size(); j++)
		if (m_scene.m_lights[j]->m_coef>1.0f)
			m_scene.m_lights[j]->m_coef /= coeftot;
		if (m_ambient > 1.0f)
			m_ambient /= coeftot;
	}*/
	// normalisation des coefs reflec/refrac
	/*for (j=0; j<m_scene.m_materials.size(); j++)
	{
		coeftot = m_scene.m_materials[j]->kReflec + m_scene.m_materials[j]->kRefrac;
		if (coeftot > 1.0f)
		{
			m_scene.m_materials[j]->kReflec /= coeftot;
			m_scene.m_materials[j]->kRefrac /= coeftot;
		}
	}*/
	for (y=0; y<height; y++)
	{
		for (x=0; x<width; x++)
		{
			primaryray.m_origin = m_camera.getPos();
			primaryray.m_direction = m_camera.m_init+(x*vXindent)-(y*vYindent);
			primaryray.m_direction.Normalize();
			*(curpixel++) = launchRay(primaryray);
		}
	}

	// dessin des sources de lumières, assimilées à des sphères jaunes
#ifdef _DEBUG
	double DistToLight = 0.0f, DistToObj = 0.0f;
	bool bIntersepte = false;
	color jaune = {255, 255, 0 }; color rose = { 255, 0, 255 }; position inter;
	Sphere sphere("lum", tofcolor(jaune), Vector(0, 0, 0), 0.5f);
	primaryray.m_origin = m_camera.getPos();
	primaryray.m_direction = m_camera.m_init;
	curpixel = m_image;

	for (uint y=0; y<height; y++)
	{
		for (uint x=0; x<width; x++)
		{
			primaryray.m_origin = m_camera.getPos();
			primaryray.m_direction = m_camera.m_init+(x*vXindent)-(y*vYindent);
			primaryray.m_direction.Normalize();
			for (uint j=0; j<m_scene.m_lights.size(); j++)
			{
				sphere.m_origin = m_scene.m_lights[j]->m_origin;
				if (sphere.Intersect(primaryray, DistToLight, &inter))
				{
					for (uint i=0; i<m_scene.m_objects.size(); i++)
					{
						if (m_scene.m_objects[i]->Intersect(primaryray, DistToObj) && DistToObj < DistToLight)
						{
							bIntersepte = true;
							memcpy((void*)&curpixel[(y)*width+x], &rose, sizeof(color));
							break;
						}
					}
					if (!bIntersepte)
						memcpy((void*)&curpixel[(y)*width+x], &jaune, sizeof(color));
					bIntersepte = false;
				}
			}
		}
	}
#endif

	return 0;
}

fcolor RayTracer::launchRay(Ray &pRay, uchar level, double nprec)
{
	Ray lightray;
	double objDist = 0.0f, objDisttemp = 0.0f;
	Shape *FirstObj = NULL, *pShape;
	bool bIntersect = false, bTempSortant = false, bSortant = false;
	double tempdist = 0.0f, n1, n2;
	position intersect;
	position tempintersect;
	uint i=0, j=0; // compteurs de boucles
	double diff = .0f, spec = .0f; // le coefficient de luminosité à l'intersection courante
	Vector *Normale; // la normale au point d'intersection
	Vector Incident; // le vecteur incident au point d'intersection
	fcolor reflecColor = { 0.0f, 0.0f, 0.0f }, refracColor = { 0.0f, 0.0f, 0.0f }, tmpColor = { 0.0f, 0.0f, 0.0f };
	//getc(stdin);
	//if (level == 0) debug_log("\n");
	//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
	//debug_log("launchRay({%2.3f %2.3f %2.3f} => {%2.3f %2.3f %2.3f}):", pRay.m_origin.x, pRay.m_origin.y, pRay.m_origin.z, pRay.m_direction.x, pRay.m_direction.y, pRay.m_direction.z);

	// parcours des objets de la scène
	for (i=0; i<m_scene.m_objects.size(); i++)
	{
		//debug_log("%d %d %d\n", x,y,i);
		bIntersect = m_scene.m_objects[i]->Intersect(pRay, tempdist, &tempintersect, &bTempSortant);
		if ((bIntersect) && ((tempdist < objDist || objDist==0)))
		{
			// DEBUG: les rayons sortant de la sphere ne sont pas interceptés par celle-ci
			//if (level > 0 && strcmp(m_scene.m_objects[i]->m_name, "sphere_verre") == 0)
			//	continue;
			FirstObj = m_scene.m_objects[i];
			objDist = tempdist;
			bSortant = bTempSortant;
			intersect = tempintersect;
			Normale = &FirstObj->m_normal;
			//break;
		}
	}

	if (FirstObj == NULL)
		return tofcolor(ambient);
	//if ((x == 215 || x == 215) && y == 150)
	//	debug_log(" %03d:%03d intersect @%2.3f %2.3f %2.3f sur %s (%s) level%d\n", x, y, intersect.x, intersect.y, intersect.z, FirstObj->m_name, bSortant?"SORTANT":"RENTRANT", level);
	//if (bSortant) debug_log("\nSORTANT level=%02d x=%03d y=%03d (%s)", level, x, y, FirstObj->m_name);

	//FirstObj->Normal(intersect, Normale);

	if ((level++ < nrec) && ((FirstObj->m_material->kReflec > 0.0f) || (FirstObj->m_material->kRefrac > 0.0f)))
	{
		Incident = pRay.m_direction.getNormalized();
		pRay.m_origin = intersect;
		if (!bSortant // pas de réflexions à l'intérieur de l'objet
			&& FirstObj->m_material->kReflec > 0.0f)
		{
			//pRay.m_direction = 2*Normale*(Normale.dotProduct(-pRay.m_direction))+pRay.m_direction;
			pRay.m_direction = Incident.reflect(*Normale).getNormalized();
			//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
			//debug_log("  -reflexion: norm={%2.3f %2.3f %2.3f} @ {%2.3f %2.3f %2.3f}\n",Normale.x, Normale.y, Normale.z,intersect.x,intersect.y,intersect.z);
			reflecColor = launchRay(pRay, level);
		}
		if (FirstObj->m_material->kRefrac > 0.0f)
		{
			if (bSortant)
			{
				n1 = FirstObj->m_material->nIndice;
				n2 = nprec;
			}
			else
			{
				n1 = nprec;
				n2 = FirstObj->m_material->nIndice;
			}
			//if (x == 272 && y == 200) debug_log("  -refraction: norm={%2.3f %2.3f %2.3f} @ {%2.3f %2.3f %2.3f}\n\tn1=%2.3f\n\tn2=%2.3f\n\tbSortant=%d\n", Normale.x, Normale.y, Normale.z, intersect.x, intersect.y, intersect.z, n1, n2, bSortant);
			pRay.m_direction = Incident.refract(*Normale, n1, n2).getNormalized();
			if (pRay.m_direction != Vector::invalid)
			{
				//pRay.m_origin.z+=10.0f;
				//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
				//debug_log("  -refraction: norm={%2.3f %2.3f %2.3f} @ {%2.3f %2.3f %2.3f}\n",Normale.x, Normale.y, Normale.z,intersect.x,intersect.y,intersect.z);
				refracColor = launchRay(pRay, level, n1);
			}
			else
				refracColor = FirstObj->m_material->m_color;
		}
		//return tmpColor;
	}

	/*if (x == 412 && y == 357)
		debug_log("now");*/

	//// PHONG:
	// parcours de toute les lumières
	//	parcours de tous les objets
	//		si aucun objet n'est positionné entre la lumière et FirstObj alors on calcul la lumière
	//		reçue à cet endroit et on continue avec les autres objets/lumières
	for (j=0; j<m_scene.m_lights.size(); j++)
	{
		objDisttemp = 0.0f;
		bIntersect = false;
		lightray.m_origin = m_scene.m_lights[j]->m_origin;
		lightray.m_direction = intersect;
		lightray.m_direction -= lightray.m_origin;
		objDist = lightray.m_direction.Length();
		lightray.m_direction.Normalize();

		for (i=0; i<m_scene.m_objects.size(); i++)
		{
			pShape = m_scene.m_objects[i];	
			if (pShape == FirstObj) continue;
			if (pShape->Intersect(lightray, objDisttemp))
			{
				if (objDisttemp-objDist < EPSILON) // WARNING : <=
				{
					bIntersect = true;
					break;
				}
			}
		}
		if (bIntersect)
			continue;
		{
			//objDist = 1.0f;
			// #1: DIFFUSE
			if (FirstObj->m_material->kDiffus > EPSILON)
			{
				if ((x == 250 || x == 250) && y == 180)
					debug_log("erg\n");
				// FIXME: le produit scalaire est fait via -Normale, pourquoi?
				diff += lightray.m_direction.dotProduct(-*Normale) * (m_scene.m_lights[j]->m_coef/* / ((objDist*objDist) / 1000.0f)*/) * FirstObj->m_material->kDiffus;// / (objDist / 100.0f);
				if (diff < 0.0f)
					diff = 0.0f;
				/*tmpColor.r += float(diff*m_scene.m_lights[j]->m_color.r);
				tmpColor.g += float(diff*m_scene.m_lights[j]->m_color.g);
				tmpColor.b += float(diff*m_scene.m_lights[j]->m_color.b);*/
			}
			// #2: SPECULAR
			if (FirstObj->m_material->kSpecular > EPSILON)
			{
				Vector vTmp = (lightray.m_direction + pRay.m_direction).getNormalized();
				spec = pow(Normale->dotProduct(vTmp), FirstObj->m_material->kShin) * FirstObj->m_material->kSpecular;// / (objDist / 100.0f);
				spec *= (m_scene.m_lights[j]->m_coef/* / ((objDist*objDist) / 1000.0f)*/);
				//spec /= (objDist*objDist) / 75.0f;
				spec /= m_scene.m_lights.size();
				//SATURATE(spec);
				tmpColor.r += float(spec*m_scene.m_lights[j]->m_color.r);
				tmpColor.g += float(spec*m_scene.m_lights[j]->m_color.g);
				tmpColor.b += float(spec*m_scene.m_lights[j]->m_color.b);
			}
		}
	}

	diff /= m_scene.m_lights.size();
	//diff *= m_ambient;
	//SATURATE(diff);

	tmpColor.r += float(diff*FirstObj->m_material->m_color.r);
	tmpColor.g += float(diff*FirstObj->m_material->m_color.g);
	tmpColor.b += float(diff*FirstObj->m_material->m_color.b);

	_float kInd = 1 - FirstObj->m_material->kReflec - FirstObj->m_material->kRefrac;
	if (kInd < 0.0f) kInd = 0.0f;
	tmpColor.r = ((kInd*tmpColor.r) + (reflecColor.r*FirstObj->m_material->kReflec) + (refracColor.r*FirstObj->m_material->kRefrac));
	tmpColor.g = ((kInd*tmpColor.g) + (reflecColor.g*FirstObj->m_material->kReflec) + (refracColor.g*FirstObj->m_material->kRefrac));
	tmpColor.b = ((kInd*tmpColor.b) + (reflecColor.b*FirstObj->m_material->kReflec) + (refracColor.b*FirstObj->m_material->kRefrac));

	//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
	//debug_log("   color: %d %d %d\n", tmpColor.r, tmpColor.g, tmpColor.b);
	return tmpColor;
}

}
