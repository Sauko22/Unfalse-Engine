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

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();

	bool CleanUp();

	void LoadFBX(char* file_path, uint filesize, GameObject* parent);

	void Import(aiNode* node, GameObject* parent, const aiScene* scene);

	void Load_Mesh();
	void Load_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject);

	void LoadTexture(char* file_path, GameObject* gameobject);
	
	virtual void LoadTextureObject(char* file_path, int i, int k, int j);

public:
	CompMesh* compmesh;
	/*GameObject* gameobject;*/
	CompTransform* comptrans;

	ILuint textIL;

	int j;

	std::string texturepath;
};