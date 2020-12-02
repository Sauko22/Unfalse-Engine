#pragma once
#include "Module.h"
#include "Globals.h"
#include "Component.h"
#include "GameObject.h"

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

	// GameObject importer
	void Import_GameObject(CompTransform* comptrans, GameObject* gameobject);
	void ImportGameObject(CompTransform* comptrans, GameObject* gameobject);
	void SaveGameObject(const char* name);
	void LoadGameObject();

	// Add values
	JSON_Object* AddObject(JSON_Array* obj);
	JSON_Array* AddArray(JSON_Object* array, const char* name);
	void AddString(JSON_Object* array, const char* name, const char* value);
	void AddMat4x4(JSON_Array* obj, float4x4 transform);
	
	// JSON EXAMPLES
	void Insert_values(/*GameObject* parent, CompTransform* transform*/);
	void Load_values(const char* file, const char* variable_name);

public:
	JSON_Value* root_value;
	JSON_Object* root_object;
	JSON_Value* init_array;
	JSON_Array* root_array;
};

