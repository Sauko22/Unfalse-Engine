#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include <string>
#include <iostream>


struct Mesh {

	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_normal = 0; // unique vertex in VRAM
	uint num_normal = 0;
	float* normal = nullptr;



};

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();
	
	bool CleanUp();

	void Import(char* file_path);
	

public:

	Mesh* impmesh;
};