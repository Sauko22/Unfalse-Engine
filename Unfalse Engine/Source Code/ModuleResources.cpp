#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"

#include "Glew\include\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Devil/include/ilut.h"
#include "Devil/include/ilu.h"
#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

#define STB_IMAGE_IMPLEMENTATION
#include "Imgui/stb_image.h"

#include <algorithm>

ModuleResources::ModuleResources(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	_type = 0;
}

ModuleResources::~ModuleResources()
{}

bool ModuleResources::Init()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleResources::Start()
{
	LOG("Init Resources");

	GetAllAssets("Assets");

	return true;
}

bool ModuleResources::CleanUp()
{
	LOG("Destroying Resources");

	return true;
}

void ModuleResources::GetAllAssets(const char* path)
{
	std::vector<std::string> assets_list;
	std::vector<std::string> dir;

	// Get path
	std::string _dir = "";
	_dir.append(path);
	_dir += "/";

	// Get files in path
	App->filesys->DiscoverFiles(_dir.c_str(), assets_list, dir);

	// If there's another file inside, call again
	for (int i = 0; i < dir.size(); i++)
	{
		std::string _path;
		_path.append(_dir).append(dir[i]);
		GetAllAssets(_path.c_str());
	}

	for (size_t i = 0; i < assets_list.size(); i++)
	{
		// Check if it already has the meta created
		std::string meta = "";
		std::string _meta = "";
		_meta.append(path).append("/").append(assets_list[i]);

		std::string fileDir = "";
		std::string extDir = "";
		std::string filetype = "";
		App->filesys->SplitFilePath(_meta.c_str(), &fileDir, &extDir, &filetype);

		meta.append(fileDir).append(extDir).append(".meta");
		
		if (!App->filesys->Exists(meta.c_str()))
		{
			// Gameobject id
			uint id = ImportFile(_meta.c_str());
			
			// Create and save meta (save through serialization)
			if (id != 0)
			{
				if (_type == 1)
				{
					App->serialization->CreateMeta(id, meta);
				}
				else if (_type == 2)
				{
					App->serialization->CreateMetaText(id, meta);
				}
				LOG("Meta for %s created", assets_list[i].c_str());
			}
			else
			{
				LOG("Meta not created, id = 0. %s", _meta.c_str());
			}
			
		}
		else
		{
			// If meta exist, load all resources on resources list
			std::string model = App->serialization->GetModel(meta.c_str());
			App->serialization->LoadResource(model.c_str());
		}
	}
}

uint ModuleResources::ImportFile(const char* assetsFile)
{
	uint ret = 0;

	// Get type of the resource
	Resource::ResType type = Resource::ResType::NO_TYPE;
	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(assetsFile, &texname, &texname_2, &texname_3);

	if (texname_3 == "fbx" || texname_3 == "FBX")
	{
		type = Resource::ResType::MODEL;
		_type = 1;
	}
	else if (texname_3 == "png" || texname_3 == "tga")
	{
		type = Resource::ResType::TEXTURE;
		_type = 2;
	}
	else
	{
		LOG("Error with the imported file type");
		return 0;
	}

	if (type == Resource::ResType::MODEL)
	{
		Resource* resource = CreateNewResource(assetsFile, type); //Save ID, assetsFile path, libraryFile path

		char* buffer = nullptr;
		uint fileSize = 0;
		fileSize = App->filesys->Load(assetsFile, &buffer); //<-- pseudocode, load from File System

		switch (resource->Type)
		{
			case Resource::ResType::MODEL: App->fbxload->Import_Model((ResModel*)resource, fileSize, buffer); break;
		}
		
		if (App->fbxload->parentid != 0)
		{
			ret = App->fbxload->parentid;
		}
		else
		{
			LOG("Error getting id file %s", assetsFile);
			return 0;
		}

		LOG("Loaded Mesh resource %s", assetsFile);
	}
	else if (type == Resource::ResType::TEXTURE)
	{
		App->fbxload->ResImport_Texture(assetsFile);
		ret = App->serialization->textuid;
		LOG("Loaded Texture resource %s", assetsFile);
	}
	else
	{
		LOG("Error reading assets file %s", assetsFile);
		return 0;
	}

	return ret;
}

uint ModuleResources::GenerateNewUID()
{
	LCG RandomUID;
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
	case Resource::ResType::MODEL: ret = (Resource*) new ResModel(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->assetsFile = assetsPath;
		LOG("%s", ret->assetsFile.c_str());
		ret->GenLibraryPath(ret);
	}
	LOG("%i", resources.size());
	return ret;
}

// This is for internal resources
Resource* ModuleResources::CreateNewResource(Resource::ResType type, uint id)
{
	Resource* ret = nullptr;

	if (id == 1)
	{
		id = GenerateNewUID();
	}

	switch (type)
	{
	case Resource::ResType::TEXTURE: ret = (Resource*) new ResTexture(id); break;
	case Resource::ResType::MESH: ret = (Resource*) new ResMesh(id); break;
	case Resource::ResType::MODEL: ret = (Resource*) new ResModel(id); break;
	}

	if (ret != nullptr)
	{
		resources[id] = ret;
		ret->GenLibraryPath(ret);
	}
	
	return ret;
}

Resource* ModuleResources::CreateNewTempResource(Resource::ResType type, uint id)
{
	Resource* ret = nullptr;
	
	if (id == 1)
	{
		id = GenerateNewUID();
	}

	switch (type)
	{
	case Resource::ResType::TEXTURE: ret = (Resource*) new ResTexture(id); break;
	case Resource::ResType::MESH: ret = (Resource*) new ResMesh(id); break;
	case Resource::ResType::MODEL: ret = (Resource*) new ResModel(id); break;
	}

	if (ret != nullptr)
	{
		tempresources[id] = ret;
		ret->GenLibraryPath(ret);
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

	LOG("RESOURCE NOT FOUND!");

	return nullptr;
}

Resource* ModuleResources::SearchResource(uint uid)
{
	//Find if the resource is already loaded
	std::map<uint, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		return it->second;
	}

	return nullptr;
}

Resource* ModuleResources::RequestTempResource(uint uid)
{
	//Find if the resource is already loaded
	std::map<uint, Resource*>::iterator it = tempresources.find(uid);
	if (it != tempresources.end())
	{
		it->second->referenceCount++;
		return it->second;
	}

	return nullptr;
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
{}

void Resource::GenLibraryPath(Resource* resource)
{
	// Create the path of the resource to library
	std::string librarypath;

	std::string obj = std::to_string(resource->UID);

	switch (resource->Type)
	{
	case Resource::ResType::MODEL: librarypath.append("Library/Models/").append(obj); break;
	case Resource::ResType::TEXTURE: librarypath.append("Library/Textures/").append(obj); break;
	case Resource::ResType::MESH: librarypath.append("Library/Meshes/").append(obj); break;
	}

	resource->libraryFile = librarypath;
}

ResModel::ResModel(uint id) : Resource(id, MODEL)
{
	Type = MODEL;
	UID = id;

	parentid = 0;
	pos = pos.zero;
	rot = rot.identity;
	scl = scl.one;
	local_transform = local_transform.identity;

	parentResource = nullptr;
}

ResModel::~ResModel()
{}

ResMesh::ResMesh(uint id) : Resource(id, MESH)
{
	Type = MESH;
	UID = id;

	id_index = 0;
	num_index = 0;
	id_vertex = 0;
	num_vertex = 0;
	vertex = 0;
	id_normals = 0;
	num_normals = 0;
	normals = 0;
	id_tex = 0;
	num_tex = 0;
	tex = 0;
	num_faces = 0;
	mesh_path = "";
	name = "";
}

ResMesh::~ResMesh()
{

}

void ResMesh::LoadResource(ResMesh* resmesh, char* buffer)
{
	char* _cursor = buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint _ranges[4];
	uint _bytes = sizeof(_ranges);
	memcpy(_ranges, _cursor, _bytes);
	_cursor += _bytes;

	resmesh->num_index = _ranges[0];
	resmesh->num_vertex = _ranges[1];
	resmesh->num_normals = _ranges[2];
	resmesh->num_tex = _ranges[3];

	// Load indices
	_bytes = sizeof(uint) * resmesh->num_index;
	resmesh->index = new uint[resmesh->num_index];
	memcpy(resmesh->index, _cursor, _bytes);
	_cursor += _bytes;

	// Load vertexs
	_bytes = sizeof(float) * resmesh->num_vertex * 3;
	resmesh->vertex = new float[resmesh->num_vertex * 3];
	memcpy(resmesh->vertex, _cursor, _bytes);
	_cursor += _bytes;

	// Load normals
	_bytes = sizeof(float) * resmesh->num_normals * 3;
	resmesh->normals = new float[resmesh->num_normals * 3];
	memcpy(resmesh->normals, _cursor, _bytes);
	_cursor += _bytes;

	// Load textures
	_bytes = sizeof(float) * resmesh->num_tex * 2;
	resmesh->tex = new float[resmesh->num_tex * 2];
	memcpy(resmesh->tex, _cursor, _bytes);
}

void ResMesh::LoadResourceScene(GameObject* gameobject)
{
	CompMesh* compmesh = (CompMesh*)gameobject->GetComponent(Component::compType::MESH);

	if (compmesh != nullptr)
	{
		//Vertex of the mesh
		glGenBuffers(1, (GLuint*)&(compmesh->id_vertex));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, compmesh->id_vertex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * compmesh->num_vertex * 3, compmesh->vertex, GL_STATIC_DRAW);

		//Normal faces of the mesh
		glGenBuffers(1, (GLuint*)&(compmesh->id_normals));
		glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->num_normals * 3, compmesh->normals, GL_STATIC_DRAW);

		//Indices of the mesh
		glGenBuffers(1, (GLuint*)&(compmesh->id_index));
		glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_index);
		glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * compmesh->num_index, compmesh->index, GL_STATIC_DRAW);

		//Uvs of the mesh
		glGenBuffers(1, (GLuint*)&(compmesh->id_tex));
		glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_tex);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->num_tex * 2, compmesh->tex, GL_STATIC_DRAW);
	}
}

void ResMesh::SaveResource(ResMesh* mesh)
{
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[4] = { mesh->num_index, mesh->num_vertex, mesh->num_normals, mesh->num_tex };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3 + sizeof(float) * mesh->num_normals * 3 + sizeof(float) * mesh->num_tex * 2;

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(float) * mesh->num_vertex * 3;
	memcpy(cursor, mesh->vertex, bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float) * mesh->num_normals * 3;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	// Store textures
	bytes = sizeof(float) * mesh->num_tex * 2;
	memcpy(cursor, mesh->tex, bytes);

	App->filesys->Save(mesh->libraryFile.c_str(), fileBuffer, size);
}

ResTexture::ResTexture(uint id) : Resource(id, TEXTURE)
{
	Type = TEXTURE;
	UID = id;

	newtexgl = 0;
	defaultex = 0;
	textgl = 0;

	texture_h = 0;
	texture_w = 0;
	texgldata = nullptr;

	texture_path = "";
	deftexname = "";
	texname = "";
}

ResTexture::~ResTexture()
{}

void ResTexture::ImportResource(ResTexture* texture, uint& filesize, char*& buffer)
{
	filesize = App->filesys->Load(texture->assetsFile.c_str(), &buffer);

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, filesize);
}

void ResTexture::LoadResource(ResTexture* restext, char* buffer, uint filesize)
{
	ILuint textIL;

	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadL(IL_DDS, (const void*)buffer, filesize);

	restext->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	restext->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	restext->texgldata = ilGetData();

	ilDeleteImages(1, &textIL);
}

void ResTexture::LoadResourceScene(char* buffer, uint filesize, GameObject* gameobject)
{
	ILuint textIL;

	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadL(IL_DDS, (const void*)buffer, filesize);

	CompMaterial* compmaterial = (CompMaterial*)gameobject->GetComponent(Component::compType::MATERIAL);
	compmaterial->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	compmaterial->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	compmaterial->textgl = ilutGLBindTexImage();

	gameobject->actualtexgl = compmaterial->textgl;

	ilDeleteImages(1, &textIL);
}

void ModuleResources::ChangeTexture(char* buffer, uint filesize, GameObject* gameobject, const char* name)
{
	if (gameobject != nullptr)
	{
		ILuint textIL;

		ilGenImages(1, &textIL);
		ilBindImage(textIL);

		ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, filesize);

		std::string texname;
		std::string texname_2;
		std::string texname_3;

		App->filesys->SplitFilePath(name, &texname, &texname_2, &texname_3);

		CompMaterial* compmaterial = (CompMaterial*)gameobject->GetComponent(Component::compType::MATERIAL);

		if (compmaterial != nullptr)
		{
			compmaterial->newtexgl = ilutGLBindTexImage();
			compmaterial->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
			compmaterial->texture_w = ilGetInteger(IL_IMAGE_WIDTH);
			compmaterial->texname = texname_2;
		}

		ilDeleteImages(1, &textIL);
	}
}

uint64 ResTexture::SaveResource(char** fileBuffer)
{
	ILuint size = 0;
	ILubyte* data = nullptr;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5); // To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, nullptr, 0); // Get the size of the data buffer

	if (size > 0)
	{
		data = new ILubyte[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			*fileBuffer = (char*)data;
		}
	}

	return size;
}

bool ModuleResources::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}