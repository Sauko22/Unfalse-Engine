#pragma once
#include "Module.h"
#include "Application.h"
#include "Globals.h"
#include "Component.h"
#include "GameObject.h"
#include "ModuleResources.h"

// JSON Header 
#include "JSON\parson.h" 

class ModuleSerialization : public Module
{
public:

	ModuleSerialization(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleSerialization();

	bool Init();
	update_status Update();
	bool CleanUp();

	void initjson();

	// Meta Creation
	void CreateMeta(uint id, std::string meta);
	void CreateMetaText(uint id, std::string meta);

	// GameObject importer
	void Import_GameObject(ResModel* model, uint id);
	void ImportGameObject(ResModel* model);
	void SaveGameObject(uint id);
	void SaveSceneGameObject(GameObject* gameobject, uint id);
	void LoadGameObject(const char* path);
	std::string GetModel(const char* path);
	void LoadModel(const char* path);
	void LoadResource(const char* path);

	// Add values
	JSON_Object* AddObject(JSON_Array* obj);
	JSON_Array* AddArray(JSON_Object* array, const char* name);
	void AddString(JSON_Object* array, const char* name, const char* value);
	void AddFloat3(JSON_Array* obj, float3 value);
	void AddQuat(JSON_Array* obj, Quat value);
	void AddFloat(JSON_Object* obj, const char* name, double value);
	
	// JSON EXAMPLES
	void Load_values(const char* file, const char* variable_name);

public:
	JSON_Value* root_value;
	JSON_Object* root_object;
	JSON_Value* init_array;
	JSON_Array* root_array;
	uint modeluid;
	uint textuid;
	uint parentuid;

	std::vector<GameObject*> gameobject_list;
	int j, k;
	uint ruid;
};

