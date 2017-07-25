#include "SceneParser.h"

namespace SRay
{
namespace Parser
{

char TypeSectionHelper::buffer[LINE_SIZE];
const char SceneParser::ERRBUFF[] = "parametre \"%s\" inconnu";

SceneParser::SceneParser(char *filename, RayTracer *pRt, bool bClearScene)
	:ifile(NULL),cursection(NULL),bInSection(false),linectr(0),rt(pRt),curSection(NULL)
{
	tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
	tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
	scene = rt->getScene();
	ifile = fopen(filename, "r");
	if (ifile == NULL)
	{
		strcpy(errBuff, "impossible d'ouvrir le fichier specifie.");
		throw std::runtime_error(errBuff);
	}
	if (pRt && bClearScene)
		pRt->getScene()->clearScene();
}

SceneParser::~SceneParser(void)
{
	Close();
	if (cursection != NULL)
	{
		delete cursection;
		cursection = NULL;
	}
	while (!sections.empty())
	{
		delete sections.back();
		sections.pop_back();
	}
}

void SceneParser::ParseLine(const char *line)
{
	char *str, *end;
	/*
		# Commentaire
		section {
			a : vala
			b : valb
			c : "valc" # Commentaire
		}
	*/
	trimwhitespace(buffer, sizeof(buffer), line);
	// on supprime tout ce qui suit un caractère de commentaire
	end = strchr(buffer, '#');
	if (end != NULL)
		*end = '\0';
	end = buffer + strlen(buffer);
	if (strcmp(buffer, "{") == 0)
	{
		if ((bInSection) || (cursection == NULL))
		{
			sprintf(errBuff, "'{' inattendu dans la chaine \"%s\" a la ligne %d", line, linectr);
			throw std::runtime_error(errBuff);
		}
		// on a rien d'autre à faire sur cette ligne que d'ouvrir l'indicateur insection
		bInSection = true;
		return;
	}
	else if (strcmp(buffer, "}") == 0)
	{
		if (!bInSection)
		{
			sprintf(errBuff, "'}' inattendu dans la chaine \"%s\" a la ligne %d", line, linectr);
			throw std::runtime_error(errBuff);
		}
		bInSection = false;
		sections.push_back(cursection);
		cursection = NULL;
		return;
	}
	else if (buffer[0] == '#')
		return;
	else if (end==buffer)
		return; // ligne vide
	/*else if ((end-buffer) < 2)
	{
		sprintf(errBuff, "l'entree \"%s\" semble invalide a la ligne %d", line, linectr);
		throw std::runtime_error(errBuff);
	}*/
	str = buffer;
	while ((isalpha(*str) || *str == '_' || (*str >= '0' && *str <= '9')) && str<end) str++;
	trimwhitespace(str, end-str+1, str);
	end = buffer + strlen(buffer);
	if (str == buffer)
	{
		sprintf(errBuff, "caractere inattendu dans la chaine \"%s\" a la ligne %d", line, linectr);
		throw std::runtime_error(errBuff);
	}
	// après trim str doit ensuite avoir comme premier caractère:
	//		cas 1 - rien si on est à l'extérieur d'un bloc
	//		cas 2 - '{' si on est à l'extérieur d'un bloc (cas où '{' est sur la même ligne que le nom de section)
	//		cas 3 - ':' si on est dans un bloc
	// on est à l'extérieur d'une section
	if (cursection == NULL)
	{
		// cas 1 & 2
		if ((str == end) || (strcmp(str, "{") == 0))
		{
			if ((strcmp(str, "{") == 0))
				bInSection = true;
			cursection = new Section;
			cursection->type = TypeSectionHelper::toTypeSection(buffer, str-buffer);
			if (cursection->type == UNKNOWN)
			{
				delete cursection;
				cursection = NULL;
				sprintf(errBuff, "type de section inconnu \"%s\" a la ligne %d", line, linectr);
				throw std::runtime_error(errBuff);
			}
			return;
		}
	}
	else if (*str == ':') // cas 3 : on est dans une section
	{
		*str++=0;
		if ((strlen(str) <= 0) || (trimwhitespace(str, end-str+1, str) <= 0))
		{
			sprintf(errBuff, "ligne %d: la propriete \"%s\" n'a pas de valeur", linectr, buffer);
			throw std::runtime_error(errBuff);
		}
		// todo: test de valeur (et de clé)
		strtolower(buffer);
		if (!cursection->pairs.insert(pair<string, string>(buffer, str)).second)
		{
			sprintf(errBuff, "ligne %d: la propriete \"%s\" est definie deux fois", linectr, buffer);
			throw std::runtime_error(errBuff);
		}
		return;
	}
	// dans tous les autres cas la ligne n'est pas valide
	sprintf(errBuff, "caractere invalide dans la chaine \"%s\" a la ligne %d", line, linectr);
	throw std::runtime_error(errBuff);
}

void SceneParser::Load()
{
	char *end;
	if (ifile == NULL)
		return;
	linectr = 0;
	do
	{
		if (fgets(linebuffer, sizeof(linebuffer), ifile) == NULL)
			break;
		linectr++;
		// suppression des retours à la ligne en fin de chaîne
		end = linebuffer + strlen(linebuffer) - 1;
		while (((*end == 0x0A) || (*end == 0x0D)) && (end > linebuffer)) end--;
		*(end+1) = 0;
		ParseLine(linebuffer);
	}
	while ((feof(ifile) == 0) && (ferror(ifile) == 0));
	Close();
	// à la fin on vérifie qu'on était pas en train de définir une section
	if ((bInSection) || (cursection != NULL))
	{
		// on supprime la section courante, qui n'a pas encore été ajoutée
		// au vecteur de sections sinon bInSection serait à false
		if ((!bInSection) && (cursection != NULL))
		{
			delete cursection;
			cursection = NULL;
		}
		sprintf(errBuff, "fin de fichier inattendue!");
		throw std::runtime_error(errBuff);
	}

	LoadScene();

	// enfin on initialise le moteur de rendu
	rt->init();
}

void SceneParser::LoadScene()
{
	// #1: moteur de rendu
	LoadRT();
	// #2: camera
	LoadCamera();
	// #3: matériaux
	LoadMat();
	// #4: lumières ponctuelles
	LoadPLight();
	// #4: lumières d'aire
	LoadALight();
	// #5.0: objets spheres
	LoadSphere();
	// #5.1: objets plans
	LoadPlan();
	// #5.2: objets meshs
	LoadMesh();
	// #5.0: objets spheres
	LoadTriangle();
}

void SceneParser::LoadRT()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != RT)
			continue;
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (it->first == "width")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				rt->setWidth(tmpInt[0]);
			}
			else if (it->first == "height")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				rt->setHeight(tmpInt[0]);
			}
			else if (it->first == "nrec")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				rt->setNRec((ushort)tmpInt[0]);
			}
			else if (it->first == "kambient")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				rt->setkAmbient((_float)tmpDouble[0]);
			}
			else if (it->first == "ambient")
			{
				ParseTripletInt(it->second.c_str(), tmpInt);
				rt->setAmbient(tmpInt[0], tmpInt[1], tmpInt[2]);
			}
			else if (it->first == "aa_level")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				rt->setAALevel((ushort)tmpInt[0]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadCamera()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != CAMERA)
			continue;
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (it->first == "pos")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				rt->getCamera()->setPos(tmpDouble[0], tmpDouble[1], tmpDouble[2]);
			}
			else if (it->first == "dir")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				rt->getCamera()->setDirection(tmpDouble[0], tmpDouble[1], tmpDouble[2]);
			}
			else if (it->first == "vdir")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				rt->getCamera()->setDirection(Vector(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else if (it->first == "fov")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				rt->getCamera()->setFov(tmpDouble[0]);
			}
			else if (it->first == "hfov")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				rt->getCamera()->setHFov(tmpDouble[0]);
			}
			else if (it->first == "vfov")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				rt->getCamera()->setVFov(tmpDouble[0]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

bool SceneParser::LoadObject(Object *pObj)
{
	if (it->first == "name")
	{
		pObj->setName(it->second.c_str());
		return true;
	}
	return false;
}

bool SceneParser::LoadShape(Shape *pShape)
{
	uint j = 0;
	if (it->first == "mat")
	{
		for (j = 0; j<scene->m_materials.size(); j++)
		{
			if (strcmp(scene->m_materials[j]->m_name, it->second.c_str()) == 0)
			{
				pShape->setMaterial(scene->m_materials[j]);
				break;
			}
		}
		return true;
	}
	return false;
}

bool SceneParser::LoadLight(Light * pLight)
{
	if (it->first == "power")
	{
		ParseFloat(it->second.c_str(), &tmpDouble[0]);
		pLight->setPower((_float)tmpDouble[0]);
		return true;
	}
	else if (it->first == "r")
	{
		ParseInt(it->second.c_str(), &tmpInt[0]);
		pLight->setRed(tmpInt[0]);
		return true;
	}
	else if (it->first == "g")
	{
		ParseInt(it->second.c_str(), &tmpInt[0]);
		pLight->setGreen(tmpInt[0]);
		return true;
	}
	else if (it->first == "b")
	{
		ParseInt(it->second.c_str(), &tmpInt[0]);
		pLight->setBlue(tmpInt[0]);
		return true;
	}
	else if (it->first == "rgb")
	{
		ParseTripletInt(it->second.c_str(), tmpInt);
		pLight->setColor(tmpInt[0], tmpInt[1], tmpInt[2]);
		return true;
	}
	return false;
}

void SceneParser::LoadMat()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != MAT)
			continue;
		Material* tmpMat = new Material();
		scene->Add(tmpMat);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpMat))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (it->first == "r")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				tmpMat->setRed(tmpInt[0]);
			}
			else if (it->first == "g")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				tmpMat->setGreen(tmpInt[0]);
			}
			else if (it->first == "b")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				tmpMat->setBlue(tmpInt[0]);
			}
			else if (it->first == "rgb")
			{
				ParseTripletInt(it->second.c_str(), tmpInt);
				tmpMat->setColor(tmpInt[0], tmpInt[1], tmpInt[2]);
			}
			else if (it->first == "kspec")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpMat->setkSpec((_float)tmpDouble[0]);
			}
			else if (it->first == "kshin")
			{
				ParseInt(it->second.c_str(), &tmpInt[0]);
				tmpMat->setkShin(tmpInt[0]);
			}
			else if (it->first == "kdif")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpMat->setkDif((_float)tmpDouble[0]);
			}
			else if (it->first == "krefrac")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpMat->setkRefrac((_float)tmpDouble[0]);
			}
			else if (it->first == "kreflec")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpMat->setkReflec((_float)tmpDouble[0]);
			}
			else if (it->first == "n")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpMat->setIndice((_float)tmpDouble[0]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadPLight()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != PLIGHT)
			continue;
		PointLight* tmpLight = new PointLight();
		scene->Add(tmpLight);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpLight))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadLight(tmpLight))
				; // Le traitement concernant un des membres de Light a réussi
			else if (it->first == "pos")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpLight->setPos(tmpDouble[0], tmpDouble[1], tmpDouble[2]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadALight()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != ALIGHT)
			continue;
		AreaLight* tmpLight = new AreaLight();
		scene->Add(tmpLight);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpLight))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadLight(tmpLight))
				; // Le traitement concernant un des membres de Light a réussi
			else if (it->first == "normal")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpLight->setNormal(Vector(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else if (it->first == "size")
			{
				ParseFloat(it->second.c_str(), tmpDouble);
				tmpLight->setSize(float(tmpDouble[0]));
			}
			else if (it->first == "pos")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpLight->setPos(tmpDouble[0], tmpDouble[1], tmpDouble[2]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadSphere()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != SPHERE)
			continue;
		Sphere* tmpSphere = new Sphere();
		scene->Add(tmpSphere);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpSphere))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadShape(tmpSphere))
				; // Le traitement concernant un des membres de Shape a réussi
			else if (it->first == "radius")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpSphere->setRadius(tmpDouble[0]);
			}
			else if (it->first == "center")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpSphere->setCenter(position(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadPlan()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != PLAN)
			continue;
		Plan* tmpPlan = new Plan();
		scene->Add(tmpPlan);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpPlan))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadShape(tmpPlan))
				; // Le traitement concernant un des membres de Shape a réussi
			else if (it->first == "normal")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpPlan->setNormal(Vector(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else if (it->first == "distance")
			{
				ParseFloat(it->second.c_str(), &tmpDouble[0]);
				tmpPlan->setDistance((_float)tmpDouble[0]);
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadMesh()
{
	uint i = 0, j = 0;
	for (i=sections.size()-1; (int)i>=0 ; i--)
	{
		curSection = sections[i];
		if (curSection->type != MESH)
			continue;
		Mesh* tmpMesh = new Mesh();
		scene->Add(tmpMesh);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpMesh))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadShape(tmpMesh))
				; // Le traitement concernant un des membres de Shape a réussi
			else if (it->first == "file")
			{
				tmpMesh->Load(const_cast<char*>(it->second.c_str()));
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase (sections.begin()+i);
	}
}

void SceneParser::LoadTriangle()
{
	uint i = 0, j = 0;
	for (i = sections.size() - 1; (int)i >= 0; i--)
	{
		curSection = sections[i];
		if (curSection->type != TRIANGLE)
			continue;
		Triangle* tmpTriangle = new Triangle();
		scene->Add(tmpTriangle);
		it = curSection->pairs.begin();
		while (it != curSection->pairs.end())
		{
			tmpDouble[0] = tmpDouble[1] = tmpDouble[2] = 0.0f;
			tmpInt[0] = tmpInt[1] = tmpInt[2] = 0;
			if (LoadObject(tmpTriangle))
				; // Le traitement concernant un des membres d'Object a réussi
			else if (LoadShape(tmpTriangle))
				; // Le traitement concernant un des membres de Shape a réussi
			else if (it->first == "vert0")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpTriangle->setVert(0, position(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else if (it->first == "vert1")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpTriangle->setVert(1, position(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else if (it->first == "vert2")
			{
				ParseTripletFloat(it->second.c_str(), tmpDouble);
				tmpTriangle->setVert(2, position(tmpDouble[0], tmpDouble[1], tmpDouble[2]));
			}
			else
			{
				sprintf(errBuff, ERRBUFF, it->first.c_str());
				throw std::runtime_error(errBuff);
			}
			curSection->pairs.erase((it++)->first);
		}
		sections.erase(sections.begin() + i);
	}
}

void SceneParser::Close()
{
	if (ifile != NULL)
	{
		fclose(ifile);
		ifile = NULL;
	}
}
}
}
