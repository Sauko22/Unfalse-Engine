#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

typedef unsigned int GLuint;

struct GameObject
{
	virtual void showInspectorWin();
	//virtual void RenderGameObject() const;

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

	bool objSelected = false;
	bool ObjrenderActive = true;
	bool ObjtexActive = true;
	bool ObjnormActive = false;
	bool ObjdefauTex = false;
};

struct EmptyGameObject
{
	virtual void CreateEmptyGameObject();
	virtual void RenderEmptyGameObject() const;
	virtual void showEmptyInspectorWin();

	std::string name = " ";
	uint gameObjects = 0;

	std::vector<GameObject*> gameobject_list;

	bool emptySelected = false;
	bool emptyrenderActive = true;
	bool emptytexActive = true;
	bool emptydefauTex = false;
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
	std::vector<EmptyGameObject*> emptygameobject_list;

	std::vector<GameObject*> temp_gameobj_list;

	char* texture1;
	char* texture2;
};