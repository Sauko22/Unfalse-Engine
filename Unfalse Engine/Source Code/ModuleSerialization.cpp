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

	// Init json values
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	init_array = json_value_init_array();
	root_array = json_value_get_array(init_array);

	json_object_set_value(root_object, "GameObject", init_array);

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

void ModuleSerialization::Import_GameObject(CompTransform* comptrans, GameObject* gameobject)
{
	ImportGameObject(comptrans, gameobject);
	SaveGameObject("BakerHouse");
	LoadGameObject();
}

void ModuleSerialization::ImportGameObject(CompTransform* comptrans, GameObject* gameobject)
{
	JSON_Object* obj = AddObject(root_array);

	// GameObject name
	AddString(obj, "Name", gameobject->name.c_str());

	// Parent name
	if (gameobject->parentGameObject->name == " ")
	{
		AddString(obj, "Parent", "root");
	}
	else
	{
		AddString(obj, "Parent", gameobject->parentGameObject->name.c_str());
	}

	// Transforms
	JSON_Array* Transform = App->serialization->AddArray(obj, "Transform");
	AddMat4x4(Transform, comptrans->local_transform);

	// Mesh
	CompMesh* compmesh = (CompMesh*)gameobject->GetComponent(Component::compType::MESH);
	CompMaterial* compmaterial = (CompMaterial*)gameobject->GetComponent(Component::compType::MATERIAL);

	if (compmesh == nullptr)
	{
		AddString(obj, "Mesh", "No mesh");
	}
	else
	{
		AddString(obj, "Mesh_Name", compmesh->name.c_str());
		//AddString(obj, "Mesh_Path", compmesh->mpath.c_str());
	}

	// Texture
	if (compmaterial == nullptr)
	{
		AddString(obj, "Texture", "No texture");
	}
	else
	{
		AddString(obj, "Texture_Name", compmaterial->texname.c_str());
		//AddString(obj, "Texture_Path", compmesh->tpath.c_str());
	}
}

void ModuleSerialization::SaveGameObject(const char* name)
{
	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(root_value);
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	std::string _name;
	_name.append("Library/Models/").append(name);

	App->filesys->Save(_name.c_str(), serialized_string, size, false);
	json_free_serialized_string(serialized_string);
}

void ModuleSerialization::LoadGameObject()
{

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

JSON_Object* ModuleSerialization::AddObject(JSON_Array* obj)
{
	JSON_Value* init_object = json_value_init_object();
	JSON_Object* _obj = json_value_get_object(init_object);

	json_array_append_value(obj, init_object);

	return _obj;
}

JSON_Array* ModuleSerialization::AddArray(JSON_Object* array, const char* name)
{
	JSON_Value* init_array = json_value_init_array();
	JSON_Array* _array = json_value_get_array(init_array);

	json_object_set_value(array, name, init_array);

	return _array;
}

void ModuleSerialization::AddString(JSON_Object* array, const char* name, const char* value)
{
	json_object_set_string(array, name, value);
}

void ModuleSerialization::AddMat4x4(JSON_Array* obj, float4x4 transform)
{
	float3 translation, scaling;
	Quat rotation;

	transform.Decompose(translation, rotation, scaling);
	json_array_append_number(obj, translation.x);
	json_array_append_number(obj, translation.y);
	json_array_append_number(obj, translation.z);
	json_array_append_number(obj, rotation.x);
	json_array_append_number(obj, rotation.y);
	json_array_append_number(obj, rotation.z);
	json_array_append_number(obj, rotation.w);
	json_array_append_number(obj, scaling.x);
	json_array_append_number(obj, scaling.y);
	json_array_append_number(obj, scaling.z);
}