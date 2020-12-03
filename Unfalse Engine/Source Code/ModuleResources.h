#pragma once
#include "Module.h"
#include "Globals.h"

#include <map>

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
	char* GenLibraryPath(Resource* resource);
	uint GetID();

public:
	const char* assetsFile;
	char* libraryFile;
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
	void GetAllAssets();
	void CreateMeta(PathNode node, uint64& assetID);

public:
	//uint Find(const char* file_in_assets) const;
	uint ImportFile(const char* new_file_in_assets);
	uint GenerateNewUID();
	//const Resource* RequestResource(uint uid) const;
	Resource* RequestResource(uint uid);
	//void ReleaseResource(uint uid);

private:
	Resource* CreateNewResource(const char* assetsFile, Resource::ResType type);

private:
	std::map<uint, Resource*> resources;
};

class ResModel : public Resource
{
public:
	ResModel(uint uid);
	virtual ~ResModel();

public:

};

class ResMesh : public Resource
{
public:
	ResMesh(uint uid);
	virtual ~ResMesh();

public:

};

class ResTexture : public Resource
{
public:
	ResTexture(uint uid);
	virtual ~ResTexture();

public:

};



