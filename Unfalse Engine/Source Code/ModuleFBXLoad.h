#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "Vector.h"
#include "Component.h"
#include "ModuleResources.h"
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

	void Import(aiNode* node, GameObject* parent, const aiScene* scene, uint id);

	// Model
	void Import_Model(ResModel* resource, uint fileSize, char* buffer);
	void ImportModel(aiNode* node, ResModel* parent, const aiScene* scene, uint id);
	void Save_Model();

	// Texture
	void ResImport_Texture(const char* assetspath);

	// Meshes
	void Import_Mesh();
	void Load_Mesh();
	void Save_Mesh(std::string name);
	void LoadMesh(std::string name, char* buffer);

	// Textures
	void Import_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject);
	void ImportTexture(std::string texname_2, std::string texname_3, uint &filesize, char* &buffer);
	uint64 SaveTexture(aiMesh* ourMesh, char** fileBuffer);
	void LoadTexture(char* buffer, uint filesize, GameObject* gameobject);
	void LoadTextureObject(char* buffer, uint filesize, GameObject* gameobject, const char* name);

public:
	CompMesh* compmesh;
	CompTransform* comptrans;

	int j, k;
	bool texinlibrary;

	std::string file_path;
	std::string mesh_path;
	bool onlibrary;
	uint parentid;
};