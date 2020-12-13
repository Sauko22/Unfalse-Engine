#include "Globals.h"
#include "Application.h"
#include "ModuleSerialization.h"

ModuleSerialization::ModuleSerialization(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	modeluid = 0;
	j = 0;
	k = 0;
	ruid = 0;
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

void ModuleSerialization::initjson()
{
	// Init json values
	root_value = json_value_init_object();
	root_object = json_value_get_object(root_value);
	init_array = json_value_init_array();
	root_array = json_value_get_array(init_array);

	json_object_set_value(root_object, "GameObject", init_array);
}

void ModuleSerialization::Import_GameObject(ResModel* model, uint id)
{
	ImportGameObject(model);
	SaveGameObject(id);
	
}

void ModuleSerialization::ImportGameObject(ResModel* model)
{
	JSON_Object* obj = AddObject(root_array);

	// GameObject name
	AddString(obj, "Name", model->name.c_str());

	// GameObject UID
	AddFloat(obj, "ID", model->UID);

	// Parent name and ID
	std::string parentname = "";
	if (model->parentResource->name == parentname)
	{
		AddString(obj, "Parent", "root");
		AddFloat(obj, "Parent ID", 0);
	}
	else
	{
		AddString(obj, "Parent", model->parentResource->name.c_str());
		AddFloat(obj, "Parent ID", model->parentResource->UID);
	}
	
	// Transforms
	JSON_Array* Position = App->serialization->AddArray(obj, "Position");
	AddFloat3(Position, model->pos);
	JSON_Array* Rot = App->serialization->AddArray(obj, "Rotation");
	AddQuat(Rot, model->rot);
	JSON_Array* Scl = App->serialization->AddArray(obj, "Scale");
	AddFloat3(Scl, model->scl);

	// Mesh
	ResMesh* resmesh = nullptr;
	for (int j = 0; j < model->component_list.size(); j++)
	{
		if (model->component_list[j]->Type == Resource::ResType::MESH)
		{
			resmesh = (ResMesh*)model->component_list[j];
		}
	}

	if (resmesh == nullptr)
	{
		AddString(obj, "Mesh", "No mesh");
		AddFloat(obj, "Mesh ID", 0);
	}
	else
	{
		AddString(obj, "Mesh_Name", resmesh->name.c_str());
		AddFloat(obj, "Mesh ID", resmesh->UID);
		//AddString(obj, "Mesh_Path", compmesh->mpath.c_str());
	}

	// Texture
	ResTexture* restex = nullptr;
	for (int j = 0; j < model->component_list.size(); j++)
	{
		if (model->component_list[j]->Type == Resource::ResType::TEXTURE)
		{
			restex = (ResTexture*)model->component_list[j];
		}
	}
	
	if (restex == nullptr)
	{
		AddString(obj, "Texture", "No texture");
		AddFloat(obj, "Texture ID", 0);
	}
	else
	{
		AddString(obj, "Texture_Name", restex->texname.c_str());
		AddFloat(obj, "Texture ID", restex->UID);
		
	}
}

void ModuleSerialization::SaveGameObject(uint id)
{
	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(root_value);
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	std::string _name;

	modeluid = id;
	if (App->scene_intro->savescene == true)
	{
		_name.append("Library/Scenes/") += std::to_string(id);
	}
	else
	{
		_name.append("Library/Models/") += std::to_string(id);
	}

	App->filesys->Save(_name.c_str(), serialized_string, size, false);
	json_free_serialized_string(serialized_string);
}

void ModuleSerialization::SaveSceneGameObject(GameObject* gameobject, uint id)
{
	JSON_Object* obj = AddObject(root_array);

	// GameObject name
	AddString(obj, "Name", gameobject->name.c_str());

	if (k == 0 && gameobject->parentGameObject == App->scene_intro->root)
	{
		AddFloat(obj, "ID", gameobject->guid);
		ruid = gameobject->guid;
		k++;
	}
	else if (k != 0 && gameobject->parentGameObject == App->scene_intro->root)
	{
		AddFloat(obj, "ID", ruid);
	}
	else
	{
		AddFloat(obj, "ID", gameobject->guid);
	}
	
	// Parent name and ID
	if (gameobject->parentGameObject == App->scene_intro->root)
	{
		AddString(obj, "Parent", "root");
		AddFloat(obj, "Parent ID", 0);
	}
	else
	{
		AddString(obj, "Parent", gameobject->parentGameObject->name.c_str());
		AddFloat(obj, "Parent ID", gameobject->parentGameObject->guid);
	}
	
	// Transforms
	CompTransform* transform = (CompTransform*)gameobject->GetComponent(Component::compType::TRANSFORM);

	JSON_Array* Position = App->serialization->AddArray(obj, "Position");
	AddFloat3(Position, transform->pos);
	JSON_Array* Rot = App->serialization->AddArray(obj, "Rotation");
	AddQuat(Rot, transform->rot);
	JSON_Array* Scl = App->serialization->AddArray(obj, "Scale");
	AddFloat3(Scl, transform->scl);

	// Mesh
	CompMesh* compmesh = (CompMesh*)gameobject->GetComponent(Component::compType::MESH);

	if (compmesh == nullptr)
	{
		AddString(obj, "Mesh", "No mesh");
		AddFloat(obj, "Mesh ID", 0);
	}
	else
	{
		AddString(obj, "Mesh_Name", compmesh->name.c_str());
		AddFloat(obj, "Mesh ID", compmesh->muid);
	}

	// Texture
	CompMaterial* comptext = (CompMaterial*)gameobject->GetComponent(Component::compType::MATERIAL);

	if (comptext == nullptr)
	{
		AddString(obj, "Texture", "No texture");
		AddFloat(obj, "Texture ID", 0);
	}
	else
	{
		AddString(obj, "Texture_Name", comptext->texname.c_str());
		AddFloat(obj, "Texture ID", comptext->tuid);
	}

	SaveGameObject(id);

	for (int i = 0; i < gameobject->children_list.size(); i++)
	{
		SaveSceneGameObject(gameobject->children_list[i], id);
	}
}

void ModuleSerialization::CreateMeta(uint id, std::string meta)
{
	// Init json values
	JSON_Value* _root_value = json_value_init_object();
	JSON_Object* _root_object = json_value_get_object(_root_value);
	JSON_Value* _init_array = json_value_init_array();
	JSON_Array* _root_array = json_value_get_array(_init_array);

	json_object_set_value(_root_object, "GameObject", _init_array);

	ResModel* resmodel = (ResModel*)App->resource->SearchResource(id);

	JSON_Object* obj = AddObject(_root_array);

	// GameObject name
	AddString(obj, "Name", resmodel->name.c_str());

	// GameObject UID
	AddFloat(obj, "ID", resmodel->UID);

	// Library path
	std::string path;
	path.append("Library/Models/") += std::to_string(modeluid);
	AddString(obj, "Library file", path.c_str());

	// Number of resources (components)
	AddFloat(obj, "Resources", resmodel->component_list.size());

	// Resources
	JSON_Array* Resources = App->serialization->AddArray(obj, "Resources");
	
	for (int i = 0; i < resmodel->children_list.size(); i++)
	{
		JSON_Value* _root_value = json_value_init_object();
		JSON_Object* _root_object = json_value_get_object(_root_value);

		// Mesh
		ResMesh* resmesh = nullptr;
		for (int j = 0; j < resmodel->children_list[i]->component_list.size(); j++)
		{
			if (resmodel->children_list[i]->component_list[j]->Type == Resource::ResType::MESH)
			{
				resmesh = (ResMesh*)resmodel->children_list[i]->component_list[j];
			}
		}
		if (resmesh != nullptr)
		{
			AddString(_root_object, "Mesh_Name", resmesh->name.c_str());
			AddFloat(_root_object, "Mesh_ID", resmesh->UID);
			AddString(_root_object, "Mesh_Library_path", resmesh->libraryFile.c_str());
		}

		// Texture
		ResTexture* restex = nullptr;
		for (int j = 0; j < resmodel->children_list[i]->component_list.size(); j++)
		{
			if (resmodel->children_list[i]->component_list[j]->Type == Resource::ResType::TEXTURE)
			{
				restex = (ResTexture*)resmodel->children_list[i]->component_list[j];
			}
		}
		if (restex != nullptr)
		{
			AddString(_root_object, "Tex_Name", restex->texname.c_str());
			AddFloat(_root_object, "Tex_ID", restex->UID);
			AddString(_root_object, "Tex_Library_path", restex->libraryFile.c_str());
		}

		json_array_append_value(Resources, _root_value);
	}

	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(_root_value);
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	App->filesys->Save(meta.c_str(), serialized_string, size, false);

	json_free_serialized_string(serialized_string);
}

void ModuleSerialization::CreateMetaText(uint id, std::string meta)
{
	// Init json values
	JSON_Value* _root_value = json_value_init_object();
	JSON_Object* _root_object = json_value_get_object(_root_value);
	JSON_Value* _init_array = json_value_init_array();
	JSON_Array* _root_array = json_value_get_array(_init_array);

	json_object_set_value(_root_object, "GameObject", _init_array);

	ResTexture* restext = (ResTexture*)App->resource->SearchResource(id);

	JSON_Object* obj = AddObject(_root_array);

	// GameObject name
	AddString(obj, "Name", restext->texname.c_str());

	// GameObject UID
	AddFloat(obj, "ID", restext->UID);

	// Library path
	std::string path;
	path.append("Library/Textures/") += std::to_string(restext->UID);
	AddString(obj, "Library file", path.c_str());

	char* serialized_string = NULL;
	serialized_string = json_serialize_to_string_pretty(_root_value);
	size_t size = sprintf(serialized_string, "%s", serialized_string);

	App->filesys->Save(meta.c_str(), serialized_string, size, false);

	json_free_serialized_string(serialized_string);
}

void ModuleSerialization::LoadGameObject(const char* path)
{
	std::string modelpath = GetModel(path);
	LoadModel(modelpath.c_str());
	gameobject_list.clear();
	j++;
}

std::string ModuleSerialization::GetModel(const char* path)
{
	JSON_Value* root_value;
	JSON_Object* object;
	JSON_Array* array;

	root_value = json_parse_file(path);
	object = json_value_get_object(root_value);
	array = json_object_get_array(object, "GameObject");

	JSON_Object* array_obj = json_array_get_object(array, 0);
	std::string _path = json_object_get_string(array_obj, "Library file");
	

	return _path;
}

void ModuleSerialization::LoadResource(const char* path)
{
	JSON_Value* root_value;
	JSON_Object* object;
	JSON_Array* array;

	root_value = json_parse_file(path);
	object = json_value_get_object(root_value);
	array = json_object_get_array(object, "GameObject");

	for (int i = 0; i < json_array_get_count(array); i++)
	{
		JSON_Object* array_obj = json_array_get_object(array, i);

		ResModel* resmodel = (ResModel*)App->resource->CreateNewResource(Resource::ResType::MODEL, json_object_get_number(array_obj, "ID"));
		resmodel->name = json_object_get_string(array_obj, "Name");
		std::string modelpath;
		modelpath.append("Library/Models/") += std::to_string(resmodel->UID);
		resmodel->libraryFile = modelpath;

		resmodel->parentid = json_object_get_number(array_obj, "Parent ID");
		if (resmodel->parentid != 0)
		{
			resmodel->parentResource = (ResModel*)App->resource->SearchResource(resmodel->parentid);
			resmodel->parentResource->children_list.push_back(resmodel);
		}

		JSON_Array* Pos = json_object_get_array(array_obj, "Position");
		resmodel->pos.Set(json_array_get_number(Pos, 0), json_array_get_number(Pos, 1), json_array_get_number(Pos, 2));
		JSON_Array* Rot = json_object_get_array(array_obj, "Rotation");
		resmodel->rot.Set(json_array_get_number(Rot, 0), json_array_get_number(Rot, 1), json_array_get_number(Rot, 2), json_array_get_number(Rot, 3));
		JSON_Array* Scl = json_object_get_array(array_obj, "Scale");
		resmodel->scl.Set(json_array_get_number(Scl, 0), json_array_get_number(Scl, 1), json_array_get_number(Scl, 2));

		resmodel->local_transform = float4x4::FromTRS(resmodel->pos, resmodel->rot, resmodel->scl);

		// Resource mesh
		ResMesh* resmesh = nullptr;
		resmesh = (ResMesh*)App->resource->CreateNewTempResource(Resource::ResType::MESH, json_object_get_number(array_obj, "Mesh ID"));

		if (resmesh->UID != 0)
		{
			resmesh = (ResMesh*)App->resource->SearchResource(resmesh->UID);

			if (resmesh == nullptr)
			{
				resmesh = (ResMesh*)App->resource->CreateNewResource(Resource::ResType::MESH, json_object_get_number(array_obj, "Mesh ID"));
				resmesh->name = json_object_get_string(array_obj, "Mesh_Name");
				std::string meshpath;
				meshpath.append("Library/Meshes/") += std::to_string(resmesh->UID);
				resmesh->libraryFile = meshpath;

				uint filesize = 0;
				char* buffer = nullptr;
				filesize = App->filesys->Load(resmesh->libraryFile.c_str(), &buffer);

				if (filesize > 0)
				{
					resmesh->mesh_path = resmesh->libraryFile;
					resmesh->LoadResource(resmesh, buffer);
				}
				else
				{
					LOG("Error loading resource mesh %s", resmesh->libraryFile.c_str());
				}
			}
		}

		// Resource texture
		ResTexture* restexture = nullptr;
		restexture = (ResTexture*)App->resource->CreateNewTempResource(Resource::ResType::TEXTURE, json_object_get_number(array_obj, "Texture ID"));

		if (restexture->UID != 0)
		{
			restexture = (ResTexture*)App->resource->SearchResource(restexture->UID);

			if (restexture == nullptr)
			{
				restexture = (ResTexture*)App->resource->CreateNewResource(Resource::ResType::TEXTURE, json_object_get_number(array_obj, "Texture ID"));
				restexture->texname = json_object_get_string(array_obj, "Texture_Name");
				std::string texpath;
				texpath.append("Library/Textures/") += std::to_string(restexture->UID);
				restexture->libraryFile = texpath;

				uint filesize = 0;
				char* buffer = nullptr;
				filesize = App->filesys->Load(restexture->libraryFile.c_str(), &buffer);

				if (filesize > 0)
				{
					restexture->defaultex = App->renderer3D->texchec;
					restexture->deftexname = "Checkers";
					restexture->texture_path = restexture->libraryFile;
					restexture->LoadResource(restexture, buffer, filesize);
				}
				else
				{
					LOG("Error loading resource texture %s", restexture->libraryFile.c_str());
				}
			}
		}
	}
}

void ModuleSerialization::LoadModel(const char* path)
{
	JSON_Value* root_value;
	JSON_Object* object;
	JSON_Array* array;

	root_value = json_parse_file(path);
	object = json_value_get_object(root_value);
	array = json_object_get_array(object, "GameObject");
	
	for (int i = 0; i < json_array_get_count(array); i++)
	{
		JSON_Object* array_obj = json_array_get_object(array, i);

		GameObject* pgameobject = new GameObject(nullptr);
		ResModel* resmodel = (ResModel*)App->resource->RequestResource(json_object_get_number(array_obj, "ID"));

		pgameobject->guid = resmodel->UID;
		pgameobject->puid = resmodel->parentid;
		if (App->input->filedropped == true && pgameobject->puid != 0)
		{
			pgameobject->puid = parentuid;
		}
		pgameobject->name = resmodel->name;
		std::string obj = std::to_string(j);
		
		if (pgameobject->puid != 0)
		{
			pgameobject->parentGameObject = App->scene_intro->SearchGameObjectID(pgameobject->puid);
			pgameobject->parentGameObject->children_list.push_back(pgameobject);
		}
		else
		{
			pgameobject->parentGameObject = App->scene_intro->root;
			pgameobject->parentGameObject->children_list.push_back(pgameobject);
			std::string rootname;
			rootname.append("GameObject_").append(obj);
			pgameobject->name = rootname;

			if (App->input->filedropped == true)
			{
				pgameobject->guid = parentuid;
			}
		}
		
		CompTransform* comptrans = (CompTransform*)pgameobject->AddComponent(Component::compType::TRANSFORM);

		comptrans->pos = resmodel->pos;
		comptrans->rot = resmodel->rot;
		comptrans->scl = resmodel->scl;

		comptrans->local_transform = resmodel->local_transform;

		// Mesh
		CompMesh* compmesh = nullptr;
		compmesh = (CompMesh*)pgameobject->AddTempComponent(Component::compType::MESH);
		compmesh->muid = json_object_get_number(array_obj, "Mesh ID");

		if (compmesh->muid != 0)
		{
			compmesh = (CompMesh*)pgameobject->AddComponent(Component::compType::MESH);
			ResMesh* resmesh = (ResMesh*)App->resource->RequestResource(json_object_get_number(array_obj, "Mesh ID"));

			std::string meshpath;
			meshpath.append("Library/Meshes/") += std::to_string(resmesh->UID);
			resmesh->libraryFile = meshpath;

			uint filesize = 0;
			char* buffer = nullptr;
			filesize = App->filesys->Load(resmesh->libraryFile.c_str(), &buffer);

			if (filesize > 0)
			{
				resmesh->mesh_path = resmesh->libraryFile;
				resmesh->LoadResource(resmesh, buffer);
			}

			compmesh->muid = resmesh->UID;
			compmesh->name = resmesh->name;
			compmesh->mpath = resmesh->mesh_path;

			compmesh->num_vertex = resmesh->num_vertex;
			compmesh->vertex = resmesh->vertex;
			compmesh->num_faces = resmesh->num_faces;
			compmesh->num_index = resmesh->num_index;
			compmesh->index = resmesh->index;
			compmesh->num_normals = resmesh->num_normals;
			compmesh->normals = resmesh->normals;
			compmesh->num_tex = resmesh->num_tex;
			compmesh->tex = resmesh->tex;
			compmesh->id_vertex = resmesh->id_vertex;
			compmesh->id_index = resmesh->id_index;
			compmesh->id_normals = resmesh->id_normals;
			compmesh->id_tex = resmesh->id_tex;

			resmesh->LoadResourceScene(pgameobject);
		}

		// Texture
		CompMaterial* comptext = nullptr;
		comptext = (CompMaterial*)pgameobject->AddTempComponent(Component::compType::MATERIAL);
		comptext->tuid = json_object_get_number(array_obj, "Texture ID");

		if (comptext->tuid != 0)
		{
			comptext = (CompMaterial*)pgameobject->AddComponent(Component::compType::MATERIAL);
			ResTexture* restext = (ResTexture*)App->resource->RequestResource(json_object_get_number(array_obj, "Texture ID"));

			comptext->tuid = restext->UID;
			comptext->texname = restext->texname;
			comptext->hastext = true;
			pgameobject->ObjtexActive = true;
			comptext->texactive = true;

			comptext->defaultex = restext->defaultex;
			comptext->deftexname = restext->deftexname;
			comptext->textgl = restext->textgl;
			comptext->texture_w = restext->texture_w;
			comptext->texture_h = restext->texture_h;
			comptext->newtexgl = restext->newtexgl;
			comptext->tpath = restext->libraryFile;
			comptext->texgldata = restext->texgldata;

			uint filesize = 0;
			char* buffer = nullptr;
			filesize = App->filesys->Load(comptext->tpath.c_str(), &buffer);
			restext->LoadResourceScene(buffer, filesize, pgameobject);
		}

		gameobject_list.push_back(pgameobject);
	}
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

void ModuleSerialization::AddFloat3(JSON_Array* obj, float3 value)
{
	json_array_append_number(obj, value.x);
	json_array_append_number(obj, value.y);
	json_array_append_number(obj, value.z);
}

void ModuleSerialization::AddQuat(JSON_Array* obj, Quat value)
{
	json_array_append_number(obj, value.x);
	json_array_append_number(obj, value.y);
	json_array_append_number(obj, value.z);
	json_array_append_number(obj, value.w);
}

void ModuleSerialization::AddFloat(JSON_Object* obj, const char* name, double value)
{
	json_object_set_number(obj, name, value);
}