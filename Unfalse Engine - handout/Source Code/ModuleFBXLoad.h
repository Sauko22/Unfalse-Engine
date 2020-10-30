#pragma once
#include "Module.h"
#include "Globals.h"
#include "Vector.h"
#include "ModuleGameObject.h"
#include "Component.h"
#include <xstring>

typedef unsigned int GLuint;
typedef unsigned int ILuint;

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();

	bool CleanUp();

	void Import(char* file_path, uint filesize, char* tex_path = nullptr);

	void Load_Mesh();

	void LoadTexture(char* file_path);
	
	//void LoadTextureObject(char* file_path, int k, int i);

public:
	EmptyGameObject* emptygameobject;
	CompMesh* compmesh;
	GameObject* gameobject;

	ILuint textIL;

	int j;

	bool ResizeFBX;
};