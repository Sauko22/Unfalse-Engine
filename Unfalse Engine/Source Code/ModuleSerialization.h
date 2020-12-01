#pragma once
#include "Module.h"
#include "Globals.h"

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
	
	// JSON EXAMPLES
	void Insert_values(/*GameObject* parent, CompTransform* transform*/);
	void Save_values(JSON_Value* root_value, char* serialized_string/*, std::string name*/);
	void Load_values(const char* file, const char* variable_name);

public:
	
};

