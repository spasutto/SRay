#pragma once

#include "../SRay/Common.h"
#include "../SRay/Triangle.h"
#include "../Utility/Utility.h"
#include <stdio.h>
#include <vector>

using namespace std;
using namespace SRay::Utility;

namespace SRay
{
namespace Parser
{

class ObjFile
{
public:
	ObjFile(char *filename, bool bClearOnClosing = true);
	~ObjFile(void);

	void Load();

protected:
	void Close();
	void ParseLine(const char *line);
	void ParseVertex();
	void ParseNormal(){}
	void ParseTextureCoord(){}
	void ParseFace();

protected:
	FILE *ifile;
	char buffer[1024];
	bool bClear;
public:
	vector<position> vertices;
	vector<Triangle*> faces;
};

}
}
