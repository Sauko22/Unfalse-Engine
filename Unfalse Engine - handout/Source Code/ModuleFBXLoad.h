#pragma once
#include "Module.h"
#include "Globals.h"
#include "Vector.h"

typedef unsigned int GLuint;
typedef unsigned int ILuint;

struct Mesh
{
	virtual void RenderMesh(int i) const;

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

	uint imgID = 0;
};

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();

	bool CleanUp();

	void Import(char* file_path,int texID);

	void Load_Mesh();
	//void Draw_Mesh();

	void LoadTexture(char* file_path);

public:

	Mesh* impmesh;

	ILuint textIL;
	GLuint textgl;
	GLuint textureID;

	// Show all fbx loaded on screen
	std::vector<Mesh*> mesh_list;
};