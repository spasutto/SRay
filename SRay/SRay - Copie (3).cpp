#include "SRay.h"

//#include <exception>	// TODO : remplacer par m�canisme d'exception
#include <cstdlib>		// TODO : supprimer d�s que plus d'appels � rand()
#include <cstring>		// TODO : supprimer d�s que plus d'appels � memset()

#include <new>

namespace SRay
{

void RayTracer::init()
{
	strcpy(errBuff, "Impossible d'allouer la m�moire pour le pixel buffer");
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
:m_camera(pWidth, pHeight), kAmbient(0.1f), nrec(10)
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
	std::vector<Light*>::size_type j=0;
	_float coeftot = 0.0f;

	/*primaryray.m_origin = m_camera.getPos();
	primaryray.m_direction = Vector(0,0,1);
	*curpixel = launchRay(primaryray);
	return 0;*/

	debug_log("SRAY RENDER %dpx/%dpx\n==================================\n", width, height);

	// normalisation des lumi�res
	/*
	0.8 --> 1.0f
	x   --> x/0.8*(1/0.8)
	*/
	/*for (j = 0; j<m_scene.m_lights.size(); j++)
		coeftot += m_scene.m_lights[j]->m_coef;// *2; // 2* pour additionner les lumi�res sp�culaire ET diffuse
	coeftot += kAmbient;
	if (false && (abs(coeftot - 1.0f) > EPSILON))
	{
		for (j = 0; j<m_scene.m_lights.size(); j++)
		//if (m_scene.m_lights[j]->m_coef>1.0f)
			m_scene.m_lights[j]->m_coef /= coeftot;
		//if (m_ambient > 1.0f)
		kAmbient /= coeftot;
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
			primaryray.m_direction = m_camera.m_init + (x*vXindent) - (y*vYindent);
			/*primaryray.m_direction.x += 0.5f - 5 * (float(rand() % 100) / 100.0f);
			primaryray.m_direction.y += 0.5f - 5 * (float(rand() % 100) / 100.0f);
			primaryray.m_direction.z += 0.5f - 5 * (float(rand() % 100) / 100.0f);*/
			primaryray.m_direction.Normalize();
			*(curpixel++) = launchRay(primaryray);
		}
	}

	// dessin des sources de lumi�res, assimil�es � des sph�res jaunes
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
					for (uint i=0; i<m_scene.m_shapes.size(); i++)
					{
						if (m_scene.m_shapes[i]->Intersect(primaryray, DistToObj) && DistToObj < DistToLight)
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
	double coeff = .0f; // le coefficient de luminosit� � l'intersection courante
	Vector Normale; // la normale au point d'intersection
	Vector Incident; // le vecteur incident au point d'intersection
	fcolor reflecColor = { 0.0f, 0.0f, 0.0f }, refracColor = { 0.0f, 0.0f, 0.0f }, specColor = { 0.0f, 0.0f, 0.0f }, tmpColor = { kAmbient, kAmbient, kAmbient };// tofcolor(ambient);
	//getc(stdin);
	//if (level == 0) debug_log("\n");
	//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
	//debug_log("launchRay({%2.3f %2.3f %2.3f} => {%2.3f %2.3f %2.3f}):", pRay.m_origin.x, pRay.m_origin.y, pRay.m_origin.z, pRay.m_direction.x, pRay.m_direction.y, pRay.m_direction.z);

	// parcours des objets de la sc�ne
	for (i=0; i<m_scene.m_shapes.size(); i++)
	{
		//debug_log("%d %d %d\n", x,y,i);
		bIntersect = m_scene.m_shapes[i]->Intersect(pRay, tempdist, &tempintersect, &bTempSortant);
		if ((bIntersect) && ((tempdist < objDist || objDist==0)))
		{
			// DEBUG: les rayons sortant de la sphere ne sont pas intercept�s par celle-ci
			//if (level > 0 && strcmp(m_scene.m_shapes[i]->m_name, "sphere_verre") == 0)
			//	continue;
			FirstObj = m_scene.m_shapes[i];
			objDist = tempdist;
			bSortant = bTempSortant;
			intersect = tempintersect;
			Normale = FirstObj->m_normal;
			//break;
		}
	}

	if (FirstObj == NULL)
		return tofcolor(ambient);


	if (x == 320 && y == 240)
	{
		debug_log(" %03d:%03d intersect @%2.3f %2.3f %2.3f (%f) sur %s (%s) level%d\n", x, y, intersect.x, intersect.y, intersect.z, objDist, FirstObj->m_name, bSortant ? "SORTANT" : "RENTRANT", level);
		//FIXME: pourquoi lorsque le rayon et la normale sont colin�aires mais de sens oppos�s, l'intersection avec la sph�re ne se fait pas?
		//FirstObj = m_scene.m_shapes("sphere");
	}

	//if ((x == 215 || x == 215) && y == 150)
	//	debug_log(" %03d:%03d intersect @%2.3f %2.3f %2.3f sur %s (%s) level%d\n", x, y, intersect.x, intersect.y, intersect.z, FirstObj->m_name, bSortant?"SORTANT":"RENTRANT", level);
	//if (bSortant) debug_log("\nSORTANT level=%02d x=%03d y=%03d (%s)", level, x, y, FirstObj->m_name);

	//FirstObj->Normal(intersect, Normale);

	/*Normale.x += 0.01f - 1 * (float(rand() % 100) / 1000.0f);
	Normale.y += 0.01f - 1 * (float(rand() % 100) / 1000.0f);
	Normale.z += 0.01f - 1 * (float(rand() % 100) / 1000.0f);*/
	if (FirstObj->m_material->displacement_func != NULL)
	{
		float noiseCoefx = float(FirstObj->m_material->displacement_func(0.1 * double(tempintersect.x), 0.1 * double(tempintersect.y), 0.1 * double(tempintersect.z)));
		float noiseCoefy = float(FirstObj->m_material->displacement_func(0.1 * double(tempintersect.y), 0.1 * double(tempintersect.z), 0.1 * double(tempintersect.x)));
		float noiseCoefz = float(FirstObj->m_material->displacement_func(0.1 * double(tempintersect.z), 0.1 * double(tempintersect.x), 0.1 * double(tempintersect.y)));

		float bump = 0.3f;
		Normale.x = (1.0f - bump) * Normale.x + bump * noiseCoefx;
		Normale.y = (1.0f - bump) * Normale.y + bump * noiseCoefy;
		Normale.z = (1.0f - bump) * Normale.z + bump * noiseCoefz;

		float temp = Normale * Normale;
		if (temp != 0.0f)
		{
			temp = 1 / sqrtf(temp);
			Normale = temp * Normale;
		}
	}

	if ((level++ < nrec) && ((FirstObj->m_material->kReflec > 0.0f) || (FirstObj->m_material->kRefrac > 0.0f)))
	{
		Incident = pRay.m_direction.getNormalized();
		pRay.m_origin = intersect;
		if (!bSortant // pas de r�flexions � l'int�rieur de l'objet
			&& FirstObj->m_material->kReflec > 0.0f
			&& FirstObj->m_material->kSpecular > 0.0f)
		{
			//pRay.m_direction = 2*Normale*(Normale.dotProduct(-pRay.m_direction))+pRay.m_direction;
			pRay.m_direction = Incident.reflect(Normale).getNormalized();
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
			pRay.m_direction = Incident.refract(Normale, n1, n2).getNormalized();
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

	//// PHONG:
	// parcours de toute les lumi�res
	//	parcours de tous les objets
	//		si aucun objet n'est positionn� entre la lumi�re et FirstObj alors on calcul la lumi�re
	//		re�ue � cet endroit et on continue avec les autres objets/lumi�res
	for (j=0; j<m_scene.m_lights.size(); j++)
	{
		objDisttemp = 0.0f;
		bIntersect = false;
		lightray.m_origin = m_scene.m_lights[j]->m_origin;
		lightray.m_direction = intersect;
		lightray.m_direction -= lightray.m_origin;
		objDist = lightray.m_direction.Length();
		lightray.m_direction.Normalize();

		for (i=0; i<m_scene.m_shapes.size(); i++)
		{
			pShape = m_scene.m_shapes[i];	
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

		//objDist = 1.0f;
		// #1: DIFFUSE
		if (FirstObj->m_material->kDiffus > EPSILON)
		{
			// FIXME: le produit scalaire est fait via -Normale, pourquoi?
			coeff += lightray.m_direction.dotProduct(-Normale) * (m_scene.m_lights[j]->m_coef/* / ((objDist*objDist) / 1000.0f)*/) * FirstObj->m_material->kDiffus;// / (objDist / 100.0f);
			if (coeff < 0.0f)
				coeff = 0.0f;
			//coeff /= m_scene.m_lights.size();
			//coeff = 0.0f;
			tmpColor.r += float(coeff*m_scene.m_lights[j]->m_color.r*FirstObj->m_material->m_color.r);
			tmpColor.g += float(coeff*m_scene.m_lights[j]->m_color.g*FirstObj->m_material->m_color.g);
			tmpColor.b += float(coeff*m_scene.m_lights[j]->m_color.b*FirstObj->m_material->m_color.b);
		}
		// #2: SPECULAR
		//if (FirstObj->m_material->kSpecular > EPSILON
		{
			Vector vTmp = (lightray.m_direction + pRay.m_direction).getNormalized();
			coeff = pow(Normale.dotProduct(vTmp), FirstObj->m_material->kShin)/* * FirstObj->m_material->kSpecular*/;// / (objDist / 100.0f);
			coeff *= (m_scene.m_lights[j]->m_coef/* / ((objDist*objDist) / 1000.0f)*/);
			//spec /= (objDist*objDist) / 75.0f;
			//spec /= m_scene.m_lights.size();
			coeff = 0.0f;
			specColor.r += float(coeff*m_scene.m_lights[j]->m_color.r*FirstObj->m_material->m_color.r);
			specColor.g += float(coeff*m_scene.m_lights[j]->m_color.g*FirstObj->m_material->m_color.g);
			specColor.b += float(coeff*m_scene.m_lights[j]->m_color.b*FirstObj->m_material->m_color.b);

			// test 1 phong massal
			float reflet = 2.0f * lightray.m_direction*Normale;
			Vector phongDir = reflet*Normale - lightray.m_direction;
			coeff = MAX(phongDir*pRay.m_direction, 0.0f);
			coeff = /*FirstObj->m_material->kSpecular**/powf(coeff, FirstObj->m_material->kShin) *m_scene.m_lights[j]->m_coef;
			//coeff = 0.0f;
			specColor.r += float(coeff*m_scene.m_lights[j]->m_color.r);//*FirstObj->m_material->m_color.r);
			specColor.g += float(coeff*m_scene.m_lights[j]->m_color.g);//*FirstObj->m_material->m_color.g);
			specColor.b += float(coeff*m_scene.m_lights[j]->m_color.b);//*FirstObj->m_material->m_color.b);
			// test 2 blinn-phong massal
			Vector blinnDir = lightray.m_direction - pRay.m_direction;
			float temp = sqrtf(blinnDir * blinnDir);
			if (temp != 0.0f)
			{
				blinnDir = (1.0f / temp)*blinnDir;
				coeff = MAX(blinnDir*Normale, 0.0f);
				coeff = 0.0f;
				specColor.r += float(coeff*m_scene.m_lights[j]->m_color.r);//*FirstObj->m_material->m_color.r);
				specColor.g += float(coeff*m_scene.m_lights[j]->m_color.g);//*FirstObj->m_material->m_color.g);
				specColor.b += float(coeff*m_scene.m_lights[j]->m_color.b);//*FirstObj->m_material->m_color.b);
			}
		}
	}

	//diff /= m_scene.m_lights.size();
	//diff *= m_ambient;
	//SATURATE(diff);

	_float kInd = 1 - FirstObj->m_material->kReflec - FirstObj->m_material->kRefrac;
	if (kInd < 0.0f) kInd = 0.0f;
	reflecColor *= FirstObj->m_material->kSpecular;
	tmpColor.r = (specColor.r + ((tmpColor.r + reflecColor.r)*FirstObj->m_material->kReflec) + (refracColor.r*FirstObj->m_material->kRefrac));
	tmpColor.g = (specColor.g + ((tmpColor.g + reflecColor.g)*FirstObj->m_material->kReflec) + (refracColor.g*FirstObj->m_material->kRefrac));
	tmpColor.b = (specColor.b + ((tmpColor.b + reflecColor.b)*FirstObj->m_material->kReflec) + (refracColor.b*FirstObj->m_material->kRefrac));

	//for (uchar pouet=0; pouet<level; pouet++)debug_log(" ");
	//debug_log("   color: %d %d %d\n", tmpColor.r, tmpColor.g, tmpColor.b);
	return tmpColor;
}

}
