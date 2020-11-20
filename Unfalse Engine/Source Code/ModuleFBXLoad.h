#pragma once
#include "Module.h"
#include "Globals.h"
#include "Vector.h"
#include "Component.h"
#include <string>

typedef unsigned int GLuint;
typedef unsigned int ILuint;
struct aiNode;
struct aiScene;
struct aiMesh;
typedef unsigned __int64 uint64;

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();

	bool CleanUp();

	void LoadFBX(char* file_path, uint filesize, GameObject* parent);

	void Import(aiNode* node, GameObject* parent, const aiScene* scene);

	// Meshes
	void Load_Mesh();
	//void ImportMesh(CompMesh* mesh);
	void Save_Mesh(CompMesh* mesh);
	//void LoadMesh(CompMesh* mesh);

	// Textures
	void Import_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject, CompMesh* compmesh);
	void ImportTexture(std::string texname_2, std::string texname_3);
	uint64 SaveTexture(aiMesh* ourMesh, char** fileBuffer);
	void LoadTexture(char* buffer, uint filesize, GameObject* gameobject);
	void LoadTextureObject(char* buffer, uint filesize, GameObject* gameobject, char* name);

public:
	CompMesh* compmesh;
	CompTransform* comptrans;

	int j;

	std::string file_path;
	std::string mesh_path;
};