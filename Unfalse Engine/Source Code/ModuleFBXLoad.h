#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "Vector.h"
#include "Component.h"
#include "ModuleResources.h"
#include <string>

struct aiNode;
struct aiScene;

class ModuleFBXLoad : public Module
{
public:
	ModuleFBXLoad(Application* app, bool start_enabled = true);
	~ModuleFBXLoad();

	bool Init();

	bool CleanUp();

	// Model
	void Import_Model(ResModel* resource, uint fileSize, char* buffer);
	void ImportModel(aiNode* node, ResModel* parent, const aiScene* scene, uint id);

	// Texture
	void ResImport_Texture(const char* assetspath);
	
public:
	int k;
	bool texinlibrary;
	bool onlibrary;
	uint parentid;
};