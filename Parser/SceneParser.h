#pragma once

#include "../SRay/Common.h"
#include "../SRay/SRay.h"
#include "../Utility/Utility.h"
#include "../Utility/Perlin.h"
#include "ObjFile.h"
#include <stdio.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

using namespace std;
using namespace SRay::Utility;

namespace SRay
{
namespace Parser
{

#define LINE_SIZE 1024

typedef map<string, string> ParamMap;

enum TYPE_SECTION	{RT,CAMERA,MAT,PLIGHT,ALIGHT,SPHERE,PLAN,MESH,TRIANGLE,UNKNOWN};

#define TypeSectionCase( n ) case n: return #n
#define TypeSectionIf1( n ) if (buffer == #n) return n
#define TypeSectionIf2( n ) if (strncmp(buffer, #n, len) == 0) return n

class TypeSectionHelper
{
public:
	static char* toString(TYPE_SECTION pType)
	{
		switch (pType)
		{
		TypeSectionCase(RT);
		TypeSectionCase(CAMERA);
		TypeSectionCase(MAT);
		TypeSectionCase(PLIGHT);
		TypeSectionCase(ALIGHT);
		TypeSectionCase(SPHERE);
		TypeSectionCase(PLAN);
		TypeSectionCase(MESH);
		TypeSectionCase(TRIANGLE);
		case UNKNOWN:
		default:
			return "";
		}
	}
	static TYPE_SECTION toTypeSection(string pType)
	{
		strncpy(buffer, pType.c_str(), sizeof(buffer));
		strtoupper(buffer);
		TypeSectionIf1(RT);
		TypeSectionIf1(CAMERA);
		TypeSectionIf1(MAT);
		TypeSectionIf1(PLIGHT);
		TypeSectionIf1(ALIGHT);
		TypeSectionIf1(SPHERE);
		TypeSectionIf1(PLAN);
		TypeSectionIf1(MESH);
		TypeSectionIf1(TRIANGLE);
		return UNKNOWN;
	}
	static TYPE_SECTION toTypeSection(const char *pType, uint len=0)
	{
		if (len == 0)
			len = strlen(pType);
		strncpy(buffer, pType, sizeof(buffer));
		strtoupper(buffer);
		TypeSectionIf2(RT);
		TypeSectionIf2(CAMERA);
		TypeSectionIf2(MAT);
		TypeSectionIf2(PLIGHT);
		TypeSectionIf2(ALIGHT);
		TypeSectionIf2(SPHERE);
		TypeSectionIf2(PLAN);
		TypeSectionIf2(MESH);
		TypeSectionIf2(TRIANGLE);
		return UNKNOWN;
	}
private:
	static char buffer[LINE_SIZE];
};

typedef struct s_section
{
	TYPE_SECTION type;
	ParamMap pairs;
} Section;

class SceneParser
{
public:
	SceneParser(char *filename, RayTracer *pRt, bool bClearScene = true);
	~SceneParser(void);

	void Load();

protected:
	void Close();
	void ParseLine(const char *line);
	void LoadScene();
private:
	bool LoadObject(Object *pObj);
	bool LoadShape(Shape *pShape);
	bool LoadLight(Light *pLight);
	void LoadRT();
	void LoadCamera();
	void LoadMat();
	void LoadPLight();
	void LoadALight();
	void LoadSphere();
	void LoadPlan();
	void LoadMesh();
	void LoadTriangle();

public:
	vector<Section*> sections;
protected:
	FILE *ifile;
	RayTracer *rt;
	char buffer[LINE_SIZE], linebuffer[LINE_SIZE];
	Section *cursection; // si == NULL: pas de section en cours de création
	bool bInSection; // si true: on est entre les accolades de la section
	unsigned int linectr;
private:
	double tmpDouble[3];
	int tmpInt[3];
	Section* curSection;
	Scene* scene;
	ParamMap::const_iterator it;
	static const char ERRBUFF[];
};

}
}
