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
	catch (const std::exception &e)
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
	int tmpDbgFlag;

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
	/*
	* Set the debug-heap flag to keep freed blocks in the
	* heap's linked list - This will allow us to catch any
	* inadvertent use of freed memory
	*/
	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;
	_CrtSetDbgFlag(tmpDbgFlag);

	/*
	* Set the debug-heap flag to no longer keep freed blocks in the
	* heap's linked list and turn on Debug type allocations (CLIENT)
	*/
	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_ALLOC_MEM_DF;
	tmpDbgFlag &= ~_CRTDBG_DELAY_FREE_MEM_DF;
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
	//_CrtSetBreakAlloc(184);
#endif

	char buff[512], buff2[512], buff3[512], buff4[128], * ptr;
	clock_t start;

	srand((uint)time(0));
	double temps = 0.0f;
	if (argc <= 1)
	{
		printf("usage: %s scene1.srs [scene2.srs ...]\n", argv[0]);
		getc(stdin);
		return 0;
	}
	try
	{
		RayTracer raytracer;
		system("mkdir out 2>NUL");
		strcpy(buff, argv[1]);
		SceneParser sf(buff, &raytracer);
		sf.Load();

		start = clock();
		raytracer.Render();
		temps = ((double)(clock() - start)) / CLK_TCK;
		sprintTps(buff4, temps);
		sprintf(buff3, "temps de rendu %s\n", buff4);
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
		sprintf(buff, "%sout\\%s.tga", buff, buff2);
		sprintf(buff2, "SRAY rendu (%dx%d) : %s", raytracer.getWidth(), raytracer.getHeight(), buff4);
		TGAWriter tga(raytracer.m_image, raytracer.getWidth(), raytracer.getHeight());
		tga.print(buff2, 1, 1);
		tga.write(buff);

		sprintf(buff2, "\"%s\"", buff);
		system(buff2);
	}
	catch (std::exception& e)
	{
		printf("EXCEPTION :: %s\n", e.what());
		getc(stdin);
	}

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
