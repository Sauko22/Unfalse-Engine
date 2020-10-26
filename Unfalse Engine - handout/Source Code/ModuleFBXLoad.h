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

	void Import(char* file_path/*,char* texID*/);

	void Load_Mesh();

	void LoadTexture(char* file_path);

	std::string SubtractString(std::string str, const char* chars_to_find, bool reading_backwards, bool subtract_until_char, bool include_char);

public:
	Component* impmesh;
	GameObject* gameobject;

	ILuint textIL;

	uint j;

	bool ResizeFBX;
};