#include "ObjFile.h"

namespace SRay
{
namespace Parser
{

ObjFile::ObjFile(char *filename, bool bClearOnClosing)
	: bClear(bClearOnClosing)
{
	ifile = fopen(filename, "r");
}


ObjFile::~ObjFile(void)
{
	Close();
	while (bClear && !faces.empty())
	{
		delete faces.back();
		faces.pop_back();
	}
}

void ObjFile::ParseVertex()
{
	char *str = buffer+1;
	double fTmp[3] = {0.0f};
	position tmpPos;
	for (unsigned short i = 0; i<3; i++)
		str = ParseFloat(str, &fTmp[i]);
	tmpPos.x = fTmp[0];
	tmpPos.y = fTmp[1];
	tmpPos.z = fTmp[2];
	vertices.push_back(tmpPos);
}

// TODO: modifier cette fonction pour qu'elle prenne en compte les coordonnées textures et les normales
void ObjFile::ParseFace()
{
	char *pgroupe = buffer+1;
	unsigned int itmp[9] = {0}; // 3*{vertexpos/texturecoord/normpos}
	unsigned short i=0;
	position tmpVertices[3];
	for (pgroupe = strtok( pgroupe, " \t/" ), i=0;  pgroupe && i<(sizeof(itmp)/sizeof(int));  pgroupe = strtok( NULL, " \t/" ), i++)
		itmp[i] = atoi(pgroupe);
	switch (i)
	{
		case 9:
			if (itmp[0] <= vertices.size())
				tmpVertices[0] = vertices.at(itmp[0]-1);
			if (itmp[3] <= vertices.size())
				tmpVertices[1] = vertices.at(itmp[3]-1);
			if (itmp[6] <= vertices.size())
				tmpVertices[2] = vertices.at(itmp[6]-1);
			break;
		case 6:
			if (itmp[0] <= vertices.size())
				tmpVertices[0] = vertices.at(itmp[0]-1);
			if (itmp[2] <= vertices.size())
				tmpVertices[1] = vertices.at(itmp[2]-1);
			if (itmp[4] <= vertices.size())
				tmpVertices[2] = vertices.at(itmp[4]-1);
			break;
		case 3:
			if (itmp[0] <= vertices.size())
				tmpVertices[0] = vertices.at(itmp[0]-1);
			if (itmp[1] <= vertices.size())
				tmpVertices[1] = vertices.at(itmp[1]-1);
			if (itmp[2] <= vertices.size())
				tmpVertices[2] = vertices.at(itmp[2]-1);
			break;
		default:
			return;
	}
	
	faces.push_back(new Triangle(tmpVertices[0], tmpVertices[1], tmpVertices[2], "pouet", fblanc));
}

void ObjFile::ParseLine(const char *line)
{
	/*
		# cube.obj
		#
		o cube
		v  0.0  0.0  0.0
		vn  0.0  0.0  1.0
		vt 0.25 0.0
		f  1/11/2  7/14/2  5/12/2
	*/
	trimwhitespace(buffer, sizeof(buffer), line);
	if (strlen(buffer) <= 3)
		return;
	switch (buffer[0])
	{
	case 'v':
		if (isspace(buffer[1]))
			ParseVertex();
		else if (buffer[1] == 'n')
			ParseNormal();
		else if (buffer[1] == 't')
			ParseTextureCoord();
		break;
	case '#':
	case 'o':
	default:
		break;
	case 'f':
		ParseFace();
		break;
	}
}

void ObjFile::Load()
{
	if (ifile == NULL)
		return;

	do
	{
		if (fgets(buffer, sizeof(buffer), ifile) == NULL)
			break;
		ParseLine(buffer);
	}
	while ((feof(ifile) == 0) && (ferror(ifile) == 0));
	Close();
}

void ObjFile::Close()
{
	if (ifile != NULL)
		fclose(ifile);
}

}
}
