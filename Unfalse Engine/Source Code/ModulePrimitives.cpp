#include "Globals.h"
#include "Application.h"
#include "Primitive.h"
#include "p2Defs.h"
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
	for (uint i = 0; i < primitive_list.size(); i++)
	{
		RELEASE(primitive_list[i])
	}

	return true;
}

update_status ModulePrimitives::Update()
{
	

	return UPDATE_CONTINUE;
}

update_status ModulePrimitives::PostUpdate()
{
	// Check primitives properties
	PrimitivesProp();

	// Render all created primitives
	if (primitive_list.empty() == false)
	{
		for (int i = 0; i < primitive_list.size(); i++)
		{
			primitive_list[i]->Render();
		}
	}

	return UPDATE_CONTINUE;
}

void ModulePrimitives::CreatePlane(float posx, float posy, float posz)
{
	Plane_Primitive* plane = new Plane_Primitive;
	plane->SetPos(posx, posy, posz);
	primitive_list.push_back((Primitive*)plane);
}

void ModulePrimitives::CreateLine(vec3 origin, vec3 destination)
{
	Line_Primitive* line = new Line_Primitive;
	line->origin = origin;
	line->destination = destination;
	primitive_list.push_back((Primitive*)line);
}

void ModulePrimitives::PrimitivesProp()
{
	if (App->UI->depth == true)
	{
		glDisable(GL_DEPTH_TEST);
	}
	else
	{
		glEnable(GL_DEPTH_TEST);
	}
	if (App->UI->cull == false)
	{
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
	}
	if (App->UI->lighting == true)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}
	if (App->UI->color == true)
	{
		glEnable(GL_COLOR_MATERIAL);
	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);
	}
	if (App->UI->texture2d == true)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}