#pragma once
#include "Module.h"
#include "Globals.h"

#include "Primitive.h"

struct PhysBody3D;

class ModulePrimitives : public Module
{
public:
	ModulePrimitives(Application* app, bool start_enabled = true);
	~ModulePrimitives();

	bool Start();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	// Primitives creation
	//void CreatePoint();
	void CreateLine(float orgnx, float orgny, float orgnz, float destx, float desty, float destz);
	void CreatePlane(float posx, float posy, float posz);
	void CreateCube(float posx, float posy, float posz, float sizex, float sizey, float sizez);
	void CreateSphere(float posx, float posy, float posz, float sizex, float sizey, float sizez);
	void CreateCylinder(float posx, float posy, float posz, float rad, float height);

	// Primitives properties
	void PrimitivesProp();

	// Primitives list
	std::vector<Primitive*> primitive_list;
};
