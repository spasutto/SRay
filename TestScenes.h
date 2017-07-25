#pragma once

#include "SRay/SRay.h"
#include "Parser/ObjFile.h"

using namespace SRay;
using namespace SRay::Parser;
fcolor bleu = {0,0,255};fcolor rouge = {255,0,0};fcolor vert = {0,255,0};fcolor bleuclair = {0,155,255};fcolor orange = {255,155,0};
position centre = position(0,0,0);
class TestScenes
{
public:
	static void scene3spheres(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		Sphere *sphere_rouge = new Sphere("sphere_rouge", rouge, position(0, 0, 0), 25);
		Sphere *sphere_bleu = new Sphere("sphere_bleu", bleu, position(-22, 22, -22), 10);
		Sphere *sphere_vert = new Sphere("sphere_vert", vert, position(22, 22, -22), 10);
		scene.Add(sphere_rouge);
		scene.Add(sphere_bleu);
		scene.Add(sphere_vert);
		scene.Add(new Plan("plan", orange, Vector(0.0f, 1.0f, 0.0f), -25.0f));
		scene.Add(new PointLight("Lum1", position(0, 20.0f, -20.0f), rouge, 0.8f));
		//scene.Add(new PointLight("Lum2", position(22, 22, 22.0f), rouge, 0.3f));
		sphere_bleu->m_material->kSpecular = 0.3f;
		//sphere_vert->m_material->kSpecular = 1.0f;
		//sphere_rouge->m_material->kSpecular = 1.0f;
		sphere_vert->m_material->kRefrac = 1.0f;
		sphere_vert->m_material->nIndice = 1.52f;
		sphere_rouge->m_material->kReflec = 1.0f;
		sphere_bleu->m_material->kReflec = 1.0f;

		position a(-25.0f, -25.0f, -25.0f);
		position b(0.0f, 10.0f, 0.0f);
		position c(25.0f, 0.0f, 0.0f);
		//scene.Add(new Triangle(a,b,c,"triangle", orange));
		rt->getCamera()->setPos(50, 30.0f, -100.0f);
		//rt->getCamera()->setPos(75, 50.0f, -50.0f);
		rt->getCamera()->setDirection(centre);
		//rt->kAmbient = 0.1f;
	}

	static void scene_in_sphere(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		Sphere *sphere_rouge = new Sphere("sphere_rouge", rouge, position(0, 0, 0), 25);
		scene.Add(sphere_rouge);
		scene.Add(new Sphere("sphere_vert", vert, position(0, 0, 70), 10));

		//scene.m_shapes[0]->m_material->kRefrac = 1.0f;

		scene.Add(new PointLight("Lum3", position(50, 50, -100), rouge, 0.8f));
		rt->kAmbient = 0.2f;
		rt->getCamera()->setPos(0, 0, -1);
		rt->getCamera()->setDirection(centre);
	}

	static void test_refrac(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		Sphere *sphere_rouge = new Sphere("sphere_rouge", rouge, position(0, 0, 0), 25);
		scene.Add(sphere_rouge);scene.m_shapes[0]->m_material->nIndice = 1.52f;scene.m_shapes[0]->m_material->kRefrac = 1.0f;
		scene.Add(new Sphere("sphere_vert", vert, position(0, 0, 70), 10));
		scene.Add(new PointLight("Lum3", position(25, 25, 25), rouge, 0.8f));
		rt->kAmbient = 0.2f;
		rt->getCamera()->setPos(0, 0, -100);
		rt->getCamera()->setDirection(centre);
	}

	// pparam [0; 1[
	static void scene3spheresanim(RayTracer *rt, float pparam = 0.0f)
	{
		double x, z;
		Scene &scene = *rt->getScene();
		rt->clearScene();
		Sphere *sphere_rouge = new Sphere("sphere_rouge", rouge, position(0, 0, 0), 25);
		scene.Add(sphere_rouge);
		scene.Add(new Sphere("sphere_bleu", bleu, position(-22, 22, -22), 10));
		scene.Add(new Sphere("sphere_vert", vert, position(22, 22, -22), 10));
		scene.Add(new Plan("plan", orange, Vector(0.0f, 1.0f, 0.0f), -25.0f));

		//scene.Add(new PointLight("Lum1", position(-22, 22, -40), rouge, 0.5f));
		//scene.Add(new PointLight("Lum2", position(22, 22, -40), rouge, 0.3f));
		z = -25.0f*cos(pparam*(2*PI));
		scene.Add(new PointLight("Lum3", position(0, -z+50, z-75), rouge, 0.8f));

		x = 50.0f*sin(pparam*(2*PI)); //sin(0)=0 => x=0
		z = -120.0f*cos(pparam*(2*PI)); //cos(0)=1 => z=-100
		rt->getCamera()->setPos(x, 30.0f, z);
		rt->getCamera()->setDirection(sphere_rouge->m_origin);
	}

	static void bunny(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		//chargement d'un obj:
		char fname[] = "bunny.obj";
		static ObjFile obj = ObjFile(fname);
		obj.Load();
		printf("%s : %d vertices, %d faces\n", fname, obj.vertices.size(), obj.faces.size());
		while (!obj.faces.empty())
		{
			Triangle *curtriangle = obj.faces.back();
			curtriangle->vert[0] *= 1000.0f;
			curtriangle->vert[1] *= 1000.0f;
			curtriangle->vert[2] *= 1000.0f;
			curtriangle->setColor(255, 255, 0);
			scene.Add(curtriangle);
			obj.faces.pop_back();
		}
		rt->getCamera()->setPos(0.0f, 80.0f, -400.0f);
	}

	static void teapot(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		//chargement d'un obj:
		char fname[] = "teapot.obj";
		static ObjFile obj = ObjFile(fname);
		obj.Load();
		printf("%s : %d vertices, %d faces\n", fname, obj.vertices.size(), obj.faces.size());
		while (!obj.faces.empty())
		{
			Triangle *curtriangle = obj.faces.back();
			curtriangle->setColor(255, 255, 0);
			scene.Add(curtriangle);
			obj.faces.pop_back();
		}
		rt->kAmbient = 0.8f;
		scene.Add(new PointLight("Lum3", position(-20, 120, -550.0f), rouge, 0.2f));
		rt->getCamera()->setPos(80.0f, 120.0f, -250.0f);
		position tmpPos = position(0,-20,0);
		rt->getCamera()->setDirection(tmpPos);
	}

	static void cube(RayTracer *rt)
	{
		Scene &scene = *rt->getScene();
		rt->clearScene();
		//chargement d'un obj:
		char fname[] = "cube2.obj";
		static ObjFile obj = ObjFile(fname);
		obj.Load();
		printf("%s : %d vertices, %d faces\n", fname, obj.vertices.size(), obj.faces.size());
		while (!obj.faces.empty())
		{
			Triangle *curtriangle = obj.faces.back();
			curtriangle->setColor(255, 255, 0);
			scene.Add(curtriangle);
			obj.faces.pop_back();
		}
		rt->kAmbient = 0.8f;
		position lumpos = position(-2.0f, 2.0f, -2.0f);
		position campos = position(2.0f, 2.0f, -2.0f);
		scene.Add(new PointLight("Lum3", lumpos, rouge, 0.8f));
		rt->getCamera()->setPos(campos);
		position tmpPos = position(0.5f,0.5f,0);
		rt->getCamera()->setDirection(tmpPos);
	}

};

