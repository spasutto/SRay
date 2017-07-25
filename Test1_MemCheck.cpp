// Test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

#include <exception>
#include <time.h>
#include <typeinfo>

#include "SRay/SRay.h"
#include "Parser/SceneParser.h"

#include "TestScenes.h"

#include "./Utility/Perlin.h"
#include "./Utility/TGAWriter.h"
#include "./Utility/Random.h"

using namespace SRay;
using namespace SRay::Utility;
using namespace SRay::Parser;

void sprintTps(char* buff, double temps);

void testParse()
{
	try
	{
		RayTracer raytracer(640/2, 480/2);
		SceneParser sf("scene1.srs", &raytracer);
		//SceneParser sf("scene2_invalid.srs");
		sf.Load();
		/*for (uint i=0; i<sf.sections.size(); i++)
		{
			printf("Section %s : %d valeurs\n", TypeSectionHelper::toString(sf.sections[i]->type), sf.sections[i]->pairs.size());
			for (ParamMap::const_iterator it = sf.sections[i]->pairs.begin(); it != sf.sections[i]->pairs.end(); ++it)
				printf("\t%s:%s\n", it->first.c_str(), it->second.c_str());
		}*/
	}
	catch (std::exception &e)
	{
		printf  ("EXCEPTION :: %s\n", e.what());
		printf(errBuff);
	}
}

int main(int argc, char* argv[])
{
	/*return CalcPI_main(argc, argv);*/
	/*for (uint i = 0; i < 100; i++)
		printf("%f ", Random::Rand());
	return 0;*/
	/*int SFMT_test(int argc, char *argv[]);
	return SFMT_test(argc, argv);*/
#ifdef _DEBUG
	_CrtMemState s1, s2, s3;
	_CrtMemCheckpoint(&s1);
#endif

	char buff[512], buff2[512], buff3[512], buff4[128], *ptr;
	clock_t start;

	srand((uint)time(0));
	vector<double> vtemps;
	double temps = 0.0f;
	if (argc <= 1)
	{
		printf("usage: %s scene1.srs [scene2.srs ...]\n", argv[0]);
		getc(stdin);
		return 0;
	}
	try
	{
		//RayTracer raytracer(6800, 5100);
		RayTracer raytracer;
		system("mkdir out 2>NUL");
		for (int iter = 1; iter<2; iter++)
		{
			strcpy(buff, argv[1]);
			strcpy(buff, "C:\\boulot\\0-Projets\\sray\\bin\\scene0 - Copie.srs");
			if (iter == 1)
			{
				SceneParser sf(buff, &raytracer);
				//SceneParser sf("scene2_invalid.srs");
				sf.Load();
			}
			//raytracer.getScene()->Add(new Cube("cube", { 1, 1, 1 }, Vector(-5.0f, 0.0f, 0), Vector(5.0f, 10.0f, 0)));

			/*Shape* obj = raytracer.getScene()->m_shapes("sphere_rouge_petite");
			if (obj)
				obj->m_material->nIndice += 0.5f;*/
			/*Material *mat;
			for (uint i = 0; i < raytracer.getScene()->m_materials.size(); i++)
			{
				mat = raytracer.getScene()->m_materials[i];// ("rouge");
				if (mat)
					mat->displacement_func = perlin_noise;		// on initialise la fonction de bruit avec la fonction de perlin
			}*/

			//raytracer.getScene()->Add(new Cube());
			//TestScenes::scene3spheresanim(&raytracer, i);
			//TestScenes::scene3spheres(&raytracer);
			start = clock();
			raytracer.Render();
			temps = ((double)(clock()-start))/CLK_TCK;
			vtemps.push_back(temps);
			sprintTps(buff4, temps);
			sprintf(buff3, "temps de rendu no%03d: %s\n", iter, buff4);
			printf(buff3);
			ptr = strrchr(buff, '.');
			if (ptr != NULL)
				*ptr = 0;
			ptr = strrchr(buff, '\\');
			if (ptr != NULL)
			{
				strcpy(buff2, ptr + 1);
				*(ptr + 1) = 0;
			}
			else
			{
				strcpy(buff2, buff);
				buff[0] = 0;
			}
			//sprintf(buff, "out\\%s_%02d.tga", buff2, iter);
			sprintf(buff, "%sout\\%s.tga", buff, buff2);
			sprintf(buff2, "SRAY rendu no%02d (%dx%d) : %s", iter, raytracer.getWidth(), raytracer.getHeight(), buff4);
			TGAWriter tga(raytracer.m_image, raytracer.getWidth(), raytracer.getHeight());
			//write_tga(buff, raytracer.getWidth()*raytracer.getAALevel(), raytracer.getHeight()*raytracer.getAALevel(), raytracer.m_image_wk);
			tga.print(buff2, 1, 1);
			tga.write(buff);
		}
		temps = 0.0f;
		for (uint i=0; i< vtemps.size(); i++)
			temps += vtemps.at(i);
		temps /= vtemps.size();
		sprintTps(buff4, temps);
		printf("temps de rendu moyen: %s\n", buff4);
		sprintf(buff2, "\"%s\"", buff);
		system(buff2);
		//printf("generation terminee, appuyez sur une touche\n");getc(stdin);
	}
	catch (std::exception &e)
	{
		printf  ("EXCEPTION :: %s\n", e.what());
		getc(stdin);
	}
	//640*480
	// nouvelle méthode (launchRay)
	//temps de rendu moyen: 0.223738 s
	// ancienne méthode (do while)
	//temps de rendu moyen: 0.213678 s
	//			==> +10 ms

	//1024*768
	// nouvelle méthode (launchRay)
	//temps de rendu moyen: 0.572171 s
	// ancienne méthode (do while)
	//temps de rendu moyen: 0.557122 s
	//			==> +15 ms
	
#ifdef _DEBUG
	printf("#################################\n");
	printf("##########MEMORY CHECK###########\n");
	printf("#################################\n");
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	//_CrtMemState s1, s2, s3;
	//_CrtMemCheckpoint(&s1);
	// memory allocations take place here
	//char *pptr =(char*) malloc(8);
	//char *pptr = new char[32];
	//strcpy(pptr, "bonjour le monde");
	//printf("%s\n", pptr);
	_CrtMemCheckpoint(&s2);

	printf("#########MEM DIFFERENCE##########\n");
	if (_CrtMemDifference(&s3, &s1, &s2))
		_CrtMemDumpStatistics(&s3);
	printf("##########MEM LEAKS##############\n");
	_CrtDumpMemoryLeaks();
	printf("#################################\n");
	printf("Appuyez sur une touche pour quitter...\n");
	getc(stdin);
#endif
	return 0;
}

void sprintTps(char *buff, double temps)
{
	ushort jours = 0, heures = 0, minutes = 0, secondes = 0, millisecondes = 0;
	jours = (ushort)(temps / 86400);
	heures = (ushort)((temps / 3600) - (jours * 86400));
	minutes = (ushort)((temps / 60) - (heures * 3600) - (jours * 86400));
	secondes = (ushort)(temps - (minutes * 60) - (heures * 3600) - (jours * 86400));
	millisecondes = (ushort)(1000.0f * (temps - secondes - (minutes * 60) - (heures * 3600) - (jours * 86400)));
	sprintf(buff, "%dj %dh %dm %ds %dms", jours, heures, minutes, secondes, millisecondes);
}
