#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleResources::~ModuleResources()
{}

bool ModuleResources::Init()
{
	LOG("Init Resources");

	return true;
}

bool ModuleResources::CleanUp()
{
	LOG("Destroying Resources");

	return true;
}

uint ModuleResources::ImportFile(const char* assetsFile)
{
	uint ret = 0;

	// Get type of the resource
	Resource::ResType type;
	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(assetsFile, &texname, &texname_2, &texname_3);

	if (texname_3 == "fbx")
	{
		type = Resource::ResType::MESH;
	}
	else if (texname_3 == "png" || texname_3 == "tga")
	{
		type = Resource::ResType::TEXTURE;
	}
	else
	{
		LOG("Error with the imported file type");
	}

	Resource* resource = CreateNewResource(assetsFile, type); //Save ID, assetsFile path, libraryFile path

	char* buffer = nullptr;
	uint fileSize = 0;
	fileSize = App->filesys->Load(assetsFile, &buffer); //<-- pseudocode, load from File System
	
	switch (resource->Type) 
	{
	case Resource::ResType::TEXTURE: App->fbxload->LoadTexture(buffer, fileSize, App->scene_intro->root); break;
	case Resource::ResType::MESH: App->fbxload->LoadFBX(buffer, fileSize, App->scene_intro->root); break;
	}

	return ret;
}

uint ModuleResources::GenerateNewUID()
{
	LCG RandomUID = 0;
	uint NewUID = 0;

	NewUID = RandomUID.Int();

	return NewUID;
}

Resource* ModuleResources::CreateNewResource(const char* assetsPath, Resource::ResType type)
{
	Resource* ret = nullptr;
	uint uid = GenerateNewUID();

	switch (type)
	{
	case Resource::ResType::TEXTURE: ret = (Resource*) new ResTexture(uid); break;
	case Resource::ResType::MESH: ret = (Resource*) new ResMesh(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->assetsFile = assetsPath;
		ret->libraryFile = ret->GenLibraryPath(ret);
	}
	return ret;
}

Resource* ModuleResources::RequestResource(uint uid)
{
	//Find if the resource is already loaded
	std::map<uint, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->referenceCount++;
		return it->second;
	}
	//Find the library file (if exists) and load the custom file format
	//return TryToLoadResource();
}

Resource::Resource(uint id, Resource::ResType type)
{
	assetsFile = "";
	libraryFile = "";
	Type = type;
	UID = id;
	referenceCount = 0;
}

Resource::~Resource()
{

}

char* Resource::GenLibraryPath(Resource* resource)
{
	// Create the path of the resource to library
	// Libraryfile = "Assets/Library/..."

	return nullptr;
}

uint Resource::GetID()
{
	return UID;
}

ResMesh::ResMesh(uint id) : Resource(id, MESH)
{
	Type = MESH;
}

ResMesh::~ResMesh()
{

}

ResTexture::ResTexture(uint id) : Resource(id, TEXTURE)
{
	Type = TEXTURE;
}

ResTexture::~ResTexture()
{

}