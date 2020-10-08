#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "Glew/include/glew.h"


ModulePrimitives::ModulePrimitives(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePrimitives::~ModulePrimitives()
{}

bool ModulePrimitives::Start()
{
	LOG("Lodaing ModulePrimitives");
	bool ret = true;

	return ret;
}

bool ModulePrimitives::CleanUp()
{
	LOG("Unloading ModulePrimitives");

	return true;
}

update_status ModulePrimitives::Update()
{
	if (primitive_list.empty() == false)
	{
		int i = 0;

		for (i; i < primitive_list.size(); i++)
		{
			primitive_list[i]->Render();
			//primitive_list[i]->InnerRender();
		}
	}

	return UPDATE_CONTINUE;
}

void ModulePrimitives::CreateCube(float posx, float posy, float posz, float sizex, float sizey, float sizez)
{
	Cube* cube = new Cube;
	cube->SetPos(posx, posy, posz);
	cube->Scale(sizex, sizey, sizez);
	primitive_list.push_back((Primitive*)cube);
}

void ModulePrimitives::CreateSphere(float posx, float posy, float posz, float sizex, float sizey, float sizez)
{
	Sphere* sphere = new Sphere;
	sphere->SetPos(posx, posy, posz);
	sphere->Scale(sizex, sizey, sizez);
	primitive_list.push_back((Primitive*)sphere);
}

void ModulePrimitives::CreatePlane(float posx, float posy, float posz)
{
	Plane* plane = new Plane;
	plane->SetPos(posx, posy, posz);
	primitive_list.push_back((Primitive*)plane);
}

void ModulePrimitives::CreateCylinder(float posx, float posy, float posz, float rad, float height)
{
	Cylinder* cylinder = new Cylinder;
	cylinder->SetPos(posx, posy, posz);
	cylinder->radius = rad;
	cylinder->height = height;
	primitive_list.push_back((Primitive*)cylinder);
}

void ModulePrimitives::CreateLine(float orgnx, float orgny, float orgnz, float destx, float desty, float destz)
{
	Line* line = new Line;
	line->origin = vec3(orgnx, orgny, orgnz);
	line->destination = vec3(destx, desty, destz);
	primitive_list.push_back((Primitive*)line);
}