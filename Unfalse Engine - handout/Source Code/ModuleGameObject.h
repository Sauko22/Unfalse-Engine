#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

typedef unsigned int GLuint;

struct Component
{
	std::string name = " ";

	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_normals = 0; // unique vertex in VRAM
	uint num_normals = 0;
	float* normals = nullptr;

	uint id_tex = 0; // unique vertex in VRAM
	uint num_tex = 0;
	float* tex = nullptr;

	char* meshTexture = nullptr;
	GLuint textgl;

	GLuint defaultex;

	bool meshSelected = false;

	/*bool MeshrenderActive = true;
	bool MeshtexActive = true;
	bool MeshnormActive = false;*/
};

struct GameObject
{
	virtual void showInspectorWin();
	virtual void RenderGameObject() const;
	virtual void CreateGameObject();

	uint components = 0;
	std::string name = " ";

	std::vector<Component*> comp_list;

	bool objSelected = false;

	bool ObjrenderActive = true;
	bool ObjtexActive = true;
	bool ObjnormActive = false;

	bool ObjdefauTex = false;
};


class ModuleGameObject : public Module
{
public:
	ModuleGameObject(Application* app, bool start_enabled = true);
	~ModuleGameObject();

	bool Start();
	update_status Update();
	bool CleanUp();
	

public:
	bool active;

	// GameObject list
	std::vector<GameObject*> gameobject_list;

	std::vector<Component*> temp_comp_list;

	char* texture1;
	char* texture2;
};