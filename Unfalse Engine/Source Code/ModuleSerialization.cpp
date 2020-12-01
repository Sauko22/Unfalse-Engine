#include "Globals.h"
#include "Application.h"
#include "ModuleSerialization.h"

ModuleSerialization::ModuleSerialization(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleSerialization::~ModuleSerialization()
{}

bool ModuleSerialization::Init()
{
	LOG("Loading Serialization");
	bool ret = true;

	Insert_values();
	Load_values("Library/Models/BakerHouse", "age");

	return ret;
}

bool ModuleSerialization::CleanUp()
{

	return true;
}

update_status ModuleSerialization::Update()
{
	

	return UPDATE_CONTINUE;
}

void ModuleSerialization::Insert_values(/*GameObject* parent, CompTransform* transform*/)
{
	// Init variables
	JSON_Value* root_value = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root_value);
	char* serialized_string = nullptr;

	// Insert values
	json_object_set_string(root_object, "ID", /*parent->name.c_str()*/"BakerHouse");
	json_object_set_number(root_object, "age", 25);
	json_object_dotset_string(root_object, "address.city", "Cupertino");
	json_object_dotset_value(root_object, "contact.emails", json_parse_string("[\"email@example.com\",\"email2@example.com\"]"));

	// JSON format text
	serialized_string = json_serialize_to_string_pretty(root_value);
	puts(serialized_string);

	// Save values
	Save_values(root_value, serialized_string/*, parent->name*/);

	// Free variables
	json_free_serialized_string(serialized_string);
	json_value_free(root_value);
}

void ModuleSerialization::Save_values(JSON_Value* root_value, char* serialized_string/*, std::string name*/)
{
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	/*std::string _name;
	_name.append("Library/Models/").append(name.c_str());*/

	App->filesys->Save(/*_name.c_str()*/"Library/Models/BakerHouse", serialized_string, size, false);
}

void ModuleSerialization::Load_values(const char* file, const char* variable_name)
{
	JSON_Value* root_value;
	JSON_Object* object;
	root_value = json_parse_file(file);
	object = json_value_get_object(root_value);

	int number = 0;

	if (json_object_has_value_of_type(object, variable_name, JSONNumber))
		number = json_object_get_number(object, variable_name);

	json_value_free(root_value);
	
	LOG("%i", number);
}