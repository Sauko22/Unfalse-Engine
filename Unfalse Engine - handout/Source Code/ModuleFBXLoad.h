#pragma once
#include "Module.h"
#include "Globals.h"
#include "Vector.h"
#include "ModuleGameObject.h"
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

	void Import(char* file_path/*,uint filesize*/);

	void Load_Mesh();

	void LoadTexture(char* file_path);
	
	void LoadTextureObject(char* file_path, int k, int i);

public:
	GameObject* impmesh;
	EmptyGameObject* emptygameobject;

	ILuint textIL;

	int j;

	bool ResizeFBX;
};