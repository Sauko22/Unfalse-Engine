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
	
	void CreatePlane(float posx, float posy, float posz);
	

	// Primitives properties
	void PrimitivesProp();

	// Primitives list
	std::vector<Primitive*> primitive_list;
};
