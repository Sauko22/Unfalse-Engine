#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"

#include <map>

typedef unsigned int GLuint;
typedef unsigned int ILuint;
typedef unsigned char ILubyte;
struct aiNode;
struct aiScene;
struct aiMesh;
typedef unsigned __int64 uint64;

class Resource
{
public:

	enum ResType
	{
		NO_TYPE,
		TEXTURE,
		MESH,
		MODEL
	};

	Resource(uint uid, ResType type);
	virtual ~Resource();

public:
	void GenLibraryPath(Resource* resource);
	uint GetID();

public:
	std::string assetsFile;
	std::string libraryFile;
	ResType Type;
	uint UID;
	uint referenceCount;
};

class ModuleResources : public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleResources();

	bool Start();
	bool CleanUp();
	void GetAllAssets(const char* path);
	void CreateMeta(std::string path, uint assetID);
	void ChangeTexture(char* buffer, uint filesize, GameObject* gameobject, const char* name);

public:
	
	uint ImportFile(const char* new_file_in_assets);
	uint GenerateNewUID();
	
	Resource* RequestResource(uint uid);
	Resource* SearchResource(uint uid);
	Resource* RequestTempResource(uint uid);
	
	Resource* CreateNewResource(Resource::ResType type, uint id = 1);
	Resource* CreateNewTempResource(Resource::ResType type, uint id = 1);

private:
	Resource* CreateNewResource(const char* assetsFile, Resource::ResType type);

public:
	std::map<uint, Resource*> resources;
	std::map<uint, Resource*> tempresources;
	uint _type;
};

class ResModel : public Resource
{
public:
	ResModel(uint uid);
	virtual ~ResModel();

	void ImportResource();
	void LoadResource();
	void SaveResource();

public:
	std::vector<ResModel*> children_list;
	std::vector<Resource*> component_list;

	ResModel* parentResource;
	uint parentid;

	std::string name;

	float3 pos;
	Quat rot;
	float3 scl;
	float4x4 local_transform;
};

class ResMesh : public Resource
{
public:
	ResMesh(uint uid);
	virtual ~ResMesh();

	void ImportResource();
	void LoadResource(ResMesh* resmesh, char* buffer);
	void LoadResourceScene(GameObject* gameobject);
	void SaveResource(ResMesh* mesh);

public:
	uint id_index;
	uint num_index;
	uint* index = nullptr;

	uint id_vertex;
	uint num_vertex;
	float* vertex;

	uint id_normals;
	uint num_normals;
	float* normals;

	uint id_tex;
	uint num_tex;
	float* tex;

	uint num_faces;

	std::string name;
	std::string mesh_path;
};

class ResTexture : public Resource
{
public:
	ResTexture(uint uid);
	virtual ~ResTexture();

	void ImportResource(ResTexture* texture, uint& filesize, char*& buffer);
	void LoadResource(ResTexture* restext, char* buffer, uint filesize);
	void LoadResourceScene(char* buffer, uint filesize, GameObject* gameobject);
	uint64 SaveResource(char** fileBuffer);

public:
	GLuint textgl;
	GLuint defaultex;
	GLuint newtexgl;
	ILubyte* texgldata;

	std::string deftexname;
	std::string texname;
	std::string texture_path;

	int texture_h;
	int texture_w;
};



