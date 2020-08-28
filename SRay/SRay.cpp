#include "SRay.h"

//#include <exception>	// TODO : remplacer par mécanisme d'exception
#include <cstdlib>		// TODO : supprimer dès que plus d'appels à rand()
#include <cstring>		// TODO : supprimer dès que plus d'appels à memset()

#include <new>

namespace SRay
{

	INLINE void RayTracer::freePixelBuffer()
	{
		if (m_image != NULL)
			delete[] m_image;
		m_image = NULL;
	}

	void RayTracer::initPixelBuffer()
	{
		strcpy(errBuff, "Impossible d'allouer la mémoire pour le pixel buffer");
		try
		{
			freePixelBuffer();
			m_image = new color[getWidth()*getHeight()];
		}
		catch (std::exception ex)
		{
			strcat(errBuff, ":\r\n");
			strcat(errBuff, ex.what());
			throw std::runtime_error(errBuff);
		}
		if (m_image == NULL)
			throw std::runtime_error(errBuff);
	}

	void RayTracer::init()
	{
		if (!getWidth() || !getHeight())
			return;
		if (aa > 1)
		{
			setWidth(aa * getWidth());
			setHeight(aa * getHeight());
		}
		m_camera.init();
		initPixelBuffer();
		clearScene(false);
		bInitOK = true;
	}

	RayTracer::RayTracer(uint pWidth, uint pHeight)
		:m_camera(pWidth, pHeight), kAmbient(0.1f), nrec(10), aa(1)
	{
		debug_log("\n\nSRAY INIT ENGINE %dpx/%dpx\n==================================\n", pWidth, pHeight);
	}

	RayTracer::~RayTracer(void)
	{
		freePixelBuffer();
	}

	//TODO: replacer ces variables globales dans la fonction Render()
	uint x = 0, y = 0;
	int RayTracer::Render(void)
	{
		if (!bInitOK)
			init();
		Ray primaryray;
		color *curpixel = m_image;
		uint width = m_camera.getWidthVP(), height = m_camera.getHeightVP(); // compteurs de boucles
		Vector vXindent = m_camera.vRight*m_camera.getXIndentVP();
		Vector vYindent = m_camera.vUp*m_camera.getYIndentVP();
		std::vector<Light*>::size_type j = 0;
		_float coeftot = 0.0f;

		/*primaryray.m_origin = m_camera.getPos();
		primaryray.m_direction = Vector(0,0,1);
		*curpixel = launchRay(primaryray);
		return 0;*/

		debug_log("SRAY RENDER %dpx/%dpx\n==================================\n", width, height);

		// normalisation des lumières
		/*
		0.8 --> 1.0f
		x   --> x/0.8*(1/0.8)
		*/
		/*for (j = 0; j<m_scene.m_lights.size(); j++)
			coeftot += m_scene.m_lights[j]->m_coef;// *2; // 2* pour additionner les lumières spéculaire ET diffuse
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
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
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

		// dessin des sources de lumières, assimilées à des sphères jaunes
#ifdef _DEBUG
		double DistToLight = 0.0f, DistToObj = 0.0f;
		bool bIntersepte = false;
		color jaune = { 255, 255, 0 }; color rose = { 255, 0, 255 }; position inter;
		Sphere sphere("lum", tofcolor(jaune), Vector(0, 0, 0), 0.5f);
		primaryray.m_origin = m_camera.getPos();
		primaryray.m_direction;
		curpixel = m_image;

		for (uint y = 0; y < height; y++)
		{
			for (uint x = 0; x < width; x++)
			{
				primaryray.m_origin = m_camera.getPos();
				primaryray.m_direction = m_camera.m_init + (x*vXindent) - (y*vYindent);
				primaryray.m_direction.Normalize();
				for (uint j = 0; j < m_scene.m_lights.size(); j++)
				{
					if (m_scene.m_lights[j]->getType() == POINTLIGHT)
						sphere.m_origin = ((PointLight*)m_scene.m_lights[j])->m_origin;
					else if (m_scene.m_lights[j]->getType() == AREALIGHT)
						sphere.m_origin = ((AreaLight*)m_scene.m_lights[j])->origin;
					if (sphere.Intersect(primaryray, &DistToLight, &inter))
					{
						for (uint i = 0; i < m_scene.m_shapes.size(); i++)
						{
							if (m_scene.m_shapes[i]->Intersect(primaryray, &DistToObj) && DistToObj < DistToLight)
							{
								bIntersepte = true;
								memcpy((void*)&curpixel[(y)*width + x], &rose, sizeof(color));
								break;
							}
						}
						if (!bIntersepte)
							memcpy((void*)&curpixel[(y)*width + x], &jaune, sizeof(color));
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
		float lightVisibility = 0.0f;
		Shape *FirstObj = NULL;
		Light *curLight = NULL;
		bool bIntersect = false, bTempSortant = false, bSortant = false;
		double tempdist = 0.0f, n1, n2;
		position intersect;
		position tempintersect;
		uint i = 0, j = 0; // compteurs de boucles
		double coeff = .0f; // le coefficient de luminosité à l'intersection courante
		Vector Normale; // la normale au point d'intersection
		Vector Incident; // le vecteur incident au point d'intersection
		fcolor reflecColor = { 0.0f, 0.0f, 0.0f }, refracColor = { 0.0f, 0.0f, 0.0f }, specColor = { 0.0f, 0.0f, 0.0f }, tmpColor = { kAmbient, kAmbient, kAmbient };// tofcolor(ambient);

		if (level++ >= nrec)
			return tmpColor;

		// parcours des objets de la scène
		for (i = 0; i < m_scene.m_shapes.size(); i++)
		{
			bIntersect = m_scene.m_shapes[i]->Intersect(pRay, &tempdist, &tempintersect, &bTempSortant);
			if ((bIntersect) && ((tempdist < objDist || objDist == 0)))
			{
				FirstObj = m_scene.m_shapes[i];
				objDist = tempdist;
				bSortant = bTempSortant;
				intersect = tempintersect;
				Normale = FirstObj->m_normal;
			}
		}

		if (FirstObj == NULL)
			return tofcolor(ambient);

		// Gestion du bruit (Perlin)
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

		// Reflections/Réfractions
		if ((FirstObj->m_material->kReflec > 0.0f) || (FirstObj->m_material->kRefrac > 0.0f))
		{
			Incident = pRay.m_direction.getNormalized();
			pRay.m_origin = intersect;
			if (!bSortant // pas de réflexions à l'intérieur de l'objet
				&& FirstObj->m_material->kReflec > 0.0f
				&& FirstObj->m_material->kSpecular > 0.0f)
			{
				pRay.m_direction = Incident.reflect(Normale).getNormalized();
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
		// parcours de toute les lumières
		//	parcours de tous les objets
		//		si aucun objet n'est positionné entre la lumière et FirstObj alors on calcul la lumière
		//		reçue à cet endroit et on continue avec les autres objets/lumières
		for (j = 0; j < m_scene.m_lights.size(); j++)
		{
			curLight = m_scene.m_lights[j];
			objDisttemp = 0.0f;
			lightVisibility = curLight->CalcVisibility(&intersect, &lightray, &objDist, &m_scene.m_shapes, FirstObj);
			if (lightVisibility <= EPSILON)
				continue;

			//objDist = 1.0f;
			// #1: DIFFUSE
			if (FirstObj->m_material->kDiffus > EPSILON)
			{
				// FIXME: le produit scalaire est fait via -Normale, pourquoi?
				coeff += lightray.m_direction.dotProduct(-Normale) * (curLight->m_coef/* / ((objDist*objDist) / 1000.0f)*/) * FirstObj->m_material->kDiffus;// / (objDist / 100.0f);
				coeff = MAX(coeff, 0.0f);
				coeff /= m_scene.m_lights.size();
				//coeff = 0.0f;
				tmpColor += (lightVisibility*coeff*curLight->m_color*FirstObj->m_material->m_color);
			}
			// #2: SPECULAR
			//if (FirstObj->m_material->kSpecular > EPSILON)
			{
				Vector vTmp = (lightray.m_direction + pRay.m_direction).getNormalized();
				coeff = pow(Normale.dotProduct(vTmp), FirstObj->m_material->kShin)/* * FirstObj->m_material->kSpecular*/;// / (objDist / 100.0f);
				coeff *= (curLight->m_coef/* / ((objDist*objDist) / 1000.0f)*/);
				//spec /= (objDist*objDist) / 75.0f;
				//spec /= m_scene.m_lights.size();
				coeff = 0.0f;
				specColor += (coeff*curLight->m_color*FirstObj->m_material->m_color);

				// test 1 phong massal
				float reflet = 2.0f * lightray.m_direction*Normale;
				Vector phongDir = reflet*Normale - lightray.m_direction;
				coeff = phongDir*pRay.m_direction;
				coeff = MAX(coeff, 0.0f);
				coeff = /*FirstObj->m_material->kSpecular**/powf(coeff, FirstObj->m_material->kShin) *curLight->m_coef;
				coeff = 0.0f;
				specColor += (coeff*curLight->m_color);//*FirstObj->m_material->m_color);

				// test 2 blinn-phong massal
				Vector blinnDir = lightray.m_direction - pRay.m_direction;
				float temp = sqrtf(blinnDir * blinnDir);
				if (temp != 0.0f)
				{
					blinnDir = (1.0f / temp)*blinnDir;
					coeff = -blinnDir*Normale;
					coeff = MAX(coeff, 0.0f);
					coeff = 0.0f;
					specColor += (coeff*curLight->m_color);//*FirstObj->m_material->m_color);
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
