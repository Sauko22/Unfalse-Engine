#include "Globals.h"
#include "Application.h"
#include "ModuleFBXLoad.h"

#include "Glew\include\glew.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")




#include "Devil/include/ilut.h"
#include "Devil/include/ilu.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

#include "MathGeoLib/include/MathGeoLib.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Imgui/stb_image.h"

ModuleFBXLoad::ModuleFBXLoad(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
	compmesh = nullptr;
	comptrans = nullptr;
	j = 0;
	k = 0;
	texinlibrary = true;
	onlibrary = false;
	parentid = 0;
}

// Destructor
ModuleFBXLoad::~ModuleFBXLoad()
{
	
}

// Called before render is available
bool ModuleFBXLoad::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);


	return ret;
}

// Called before quitting
bool ModuleFBXLoad::CleanUp()
{
	LOG("Destroying 3D Renderer");

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleFBXLoad::LoadFBX(char* file_path, uint filesize, GameObject* parent)
{
	const aiScene* scene = aiImportFileFromMemory(file_path, filesize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		uint id = App->resource->GenerateNewUID();
		Import(scene->mRootNode, parent, scene, id);
		j++;
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}

// PostUpdate present buffer to screen
void ModuleFBXLoad::Import(aiNode* node, GameObject* parent, const aiScene* scene, uint id)
{
	GameObject* pgameobject = new GameObject(parent);

	std::string obj = std::to_string(j);

	std::string rootnode = "RootNode";

	if (node->mName.C_Str() == rootnode)
	{
		pgameobject->name.append("GameObject_").append(obj);
	}
	else
	{
		pgameobject->name = node->mName.C_Str();
	}
	pgameobject->guid = App->resource->GenerateNewUID();

	LOG("GameObject: %s", pgameobject->name.c_str());

	CompTransform* comptrans = (CompTransform*)pgameobject->AddComponent(Component::compType::TRANSFORM);

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	float3 posate, scalete;
	Quat rotate;

	

	node->mTransformation.Decompose(scaling, rotation, translation);


	posate.Set(translation.x, translation.y, translation.z);
	rotate.Set(rotation.x, rotation.y, rotation.z, rotation.w);
	scalete.Set(scaling.x, scaling.y, scaling.z);

	comptrans->pos = posate;
	comptrans->rot = rotate;
	comptrans->scl = scalete;

	comptrans->local_transform = float4x4::FromTRS(comptrans->pos, comptrans->rot, comptrans->scl);
	if (dynamic_cast <CompTransform*>(comptrans->gameObject->parentGameObject->GetComponent(Component::compType::TRANSFORM)) != nullptr)
	{
		comptrans->global_transform = dynamic_cast <CompTransform*>(comptrans->gameObject->parentGameObject->GetComponent(Component::compType::TRANSFORM))->global_transform * comptrans->local_transform;

	}

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		compmesh = (CompMesh*)pgameobject->AddComponent(Component::compType::MESH);

		aiMesh* ourMesh = scene->mMeshes[node->mMeshes[i]];

		compmesh->name = node->mName.C_Str();

		// Mesh importer 
		file_path = "";
		uint64 filesize = 0;
		char* buffer = nullptr;

		// The mesh is in the library
		file_path.append("Library/Meshes/").append(compmesh->name);
		filesize = App->filesys->Load(file_path.c_str(), &buffer);

		// The mesh is not in the library
		if (filesize == 0)
		{
			LOG("Lodaing mesh from assets");

			// copy vertices
			compmesh->num_vertex = ourMesh->mNumVertices;
			compmesh->vertex = new float[compmesh->num_vertex * 3];
			memcpy(compmesh->vertex, ourMesh->mVertices, sizeof(float) * compmesh->num_vertex * 3);
			LOG("New mesh with %d vertices", compmesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				compmesh->num_faces = ourMesh->mNumFaces;
				compmesh->num_index = ourMesh->mNumFaces * 3;
				compmesh->index = new uint[compmesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&compmesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (ourMesh->HasNormals())
			{
				compmesh->num_normals = ourMesh->mNumVertices;
				compmesh->normals = new float[compmesh->num_normals * 3];
				memcpy(compmesh->normals, ourMesh->mNormals, sizeof(float) * compmesh->num_normals * 3);
				LOG("New mesh with %d normal", compmesh->num_normals);
				LOG("New mesh with %d idnormal", compmesh->id_normals);
			}
			if (ourMesh->HasTextureCoords(0))
			{
				compmesh->num_tex = ourMesh->mNumVertices;
				compmesh->tex = new float[ourMesh->mNumVertices * 2];

				for (unsigned int i = 0; i < compmesh->num_tex; i++)
				{
					compmesh->tex[i * 2] = ourMesh->mTextureCoords[0][i].x;
					compmesh->tex[i * 2 + 1] = ourMesh->mTextureCoords[0][i].y;

				}
				LOG("New mesh with %d uvs", compmesh->num_tex);
			}
			Load_Mesh();

			Save_Mesh(compmesh->name);
		}
		else
		{
			LOG("Lodaing mesh from library");

			LoadMesh(compmesh->name, buffer);
			Load_Mesh();
		}
		
		// Texture importer
		Import_Texture(ourMesh, scene, pgameobject);

		
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		Import(node->mChildren[i], pgameobject, scene, id);
	}
}

void ModuleFBXLoad::Import_Mesh()
{
	
}

void ModuleFBXLoad::LoadMesh(std::string name, char* buffer)
{
	char* _cursor = buffer;
	// amount of indices / vertices / colors / normals / texture_coords
	uint _ranges[4];
	uint _bytes = sizeof(_ranges);
	memcpy(_ranges, _cursor, _bytes);
	_cursor += _bytes;

	compmesh->num_index = _ranges[0];
	compmesh->num_vertex = _ranges[1];
	compmesh->num_normals = _ranges[2];
	compmesh->num_tex = _ranges[3];

	// Load indices
	_bytes = sizeof(uint) * compmesh->num_index;
	compmesh->index = new uint[compmesh->num_index];
	memcpy(compmesh->index, _cursor, _bytes);
	_cursor += _bytes;

	// Load vertexs
	_bytes = sizeof(float) * compmesh->num_vertex * 3;
	compmesh->vertex = new float[compmesh->num_vertex * 3];
	memcpy(compmesh->vertex, _cursor, _bytes);
	_cursor += _bytes;

	// Load normals
	_bytes = sizeof(float) * compmesh->num_normals * 3;
	compmesh->normals = new float[compmesh->num_normals * 3];
	memcpy(compmesh->normals, _cursor, _bytes);
	_cursor += _bytes;

	// Load textures
	_bytes = sizeof(float) * compmesh->num_tex * 2;
	compmesh->tex = new float[compmesh->num_tex * 2];
	memcpy(compmesh->tex, _cursor, _bytes);
}

void ModuleFBXLoad::Load_Mesh()
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

void ModuleFBXLoad::Save_Mesh(std::string name)
{
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[4] = { compmesh->num_index, compmesh->num_vertex, compmesh->num_normals, compmesh->num_tex };
	uint size = sizeof(ranges) + sizeof(uint) * compmesh->num_index + sizeof(float) * compmesh->num_vertex * 3 + sizeof(float) * compmesh->num_normals * 3 + sizeof(float) * compmesh->num_tex * 2;

	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	std::string path;

	// Store indices
	bytes = sizeof(uint) * compmesh->num_index;
	memcpy(cursor, compmesh->index, bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(float) * compmesh->num_vertex * 3;
	memcpy(cursor, compmesh->vertex, bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float) * compmesh->num_normals * 3;
	memcpy(cursor, compmesh->normals, bytes);
	cursor += bytes;

	// Store textures
	bytes = sizeof(float) * compmesh->num_tex * 2;
	memcpy(cursor, compmesh->tex, bytes);

	compmesh->muid = App->resource->GenerateNewUID();

	path.append("Library/Meshes/") += std::to_string(compmesh->muid);

	

	App->filesys->Save(path.c_str(), fileBuffer, size);
}

void ModuleFBXLoad::Import_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject)
{
	if (ourMesh->HasTextureCoords(0))
	{
		// Material
		if (scene->HasMaterials())
		{
			// Active texture
			gameobject->AddComponent(Component::compType::MATERIAL);
			CompMaterial* compmaterial = (CompMaterial*)gameobject->GetComponent(Component::compType::MATERIAL);
			
			compmaterial->hastext = true;
			
			// Default texture
			compmaterial->defaultex = App->renderer3D->texchec;
			compmaterial->deftexname = "Checkers";
			gameobject->ObjtexActive = true;
			compmaterial->texactive = true;

			// Texture name
			aiMaterial* texture = nullptr;
			aiString texture_path;
			std::string texname;
			std::string texname_2;
			std::string texname_3;

			texture = scene->mMaterials[ourMesh->mMaterialIndex];
			texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

			App->filesys->SplitFilePath(texture_path.C_Str(), &texname, &texname_2, &texname_3);

			compmaterial->texname = texname_2;
			
			uint filesize = 0;
			char* buffer = nullptr;

			// Texture Import 
			ImportTexture(texname_2, texname_3, filesize, buffer);
			
			// Texture save (if it's not in the library)
			if (texinlibrary == false)
			{
				LOG("Lodaing texture from assets");

				filesize = SaveTexture(ourMesh, &buffer);

				if (filesize > 0)
				{
					compmaterial->tuid = App->resource->GenerateNewUID();

					file_path = "";
					file_path.append("Library/Textures/") += std::to_string(compmaterial->tuid);
					App->filesys->Save(file_path.c_str(), buffer, filesize);
				}
				texinlibrary = true;
			}
			else 
			{
				LOG("Lodaing texture from library");
			}

			// Texture load
			LoadTexture(buffer, filesize, gameobject);
		}
	}
}

void ModuleFBXLoad::ImportTexture(std::string texname_2, std::string texname_3, uint &filesize, char* &buffer)
{
	file_path = "";
	
	// The texture is in the library
	file_path.append("Library/Textures/").append(texname_2);
	filesize = App->filesys->Load(file_path.c_str(), &buffer);

	// The texture is not in the library
	if (filesize == 0)
	{
		buffer = nullptr;
		file_path = "";
		file_path.append("Assets/Textures/").append(texname_2).append(".").append(texname_3);
		filesize = App->filesys->Load(file_path.c_str(), &buffer);
		texinlibrary = false;
	}

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, filesize);
}

void ModuleFBXLoad::LoadTexture(char* buffer, uint filesize, GameObject* gameobject) 
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

uint64 ModuleFBXLoad::SaveTexture(aiMesh* ourMesh, char** fileBuffer)
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

void ModuleFBXLoad::LoadTextureObject(char* buffer, uint filesize, GameObject* gameobject, const char* name)
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
		compmaterial->texname = texname_2;

		compmaterial->newtexgl = ilutGLBindTexImage();
		compmaterial->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
		compmaterial->texture_w = ilGetInteger(IL_IMAGE_WIDTH);
		compmaterial->texname = texname_2;

		ilDeleteImages(1, &textIL);
	}
}

void ModuleFBXLoad::Import_Model(ResModel* resource, uint fileSize, char* buffer)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, fileSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		uint id = App->resource->GenerateNewUID();
		App->serialization->initjson();
		ImportModel(scene->mRootNode, resource, scene, id);
		k += 1;
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}

void ModuleFBXLoad::ImportModel(aiNode* node, ResModel* resmodel, const aiScene* scene, uint id)
{
	ResModel* parent = (ResModel*)App->resource->CreateNewResource(Resource::ResType::MODEL);

	parent->parentResource = resmodel;

	if (parent->parentResource != nullptr)
	{
		parent->parentResource->children_list.push_back(parent);
	}

	// Name
	std::string obj = std::to_string(k);

	std::string rootnode = "RootNode";

	if (node->mName.C_Str() == rootnode)
	{
		parent->name.append("GameObject_").append(obj);
		parentid = parent->UID;
	}
	else
	{
		parent->name = node->mName.C_Str();
		parent->parentid = parentid;
	}

	// Transforms
	aiVector3D translation, scaling;
	aiQuaternion rotation;

	float3 posate, scalete;
	Quat rotate;

	node->mTransformation.Decompose(scaling, rotation, translation);

	posate.Set(translation.x, translation.y, translation.z);
	rotate.Set(rotation.x, rotation.y, rotation.z, rotation.w);
	scalete.Set(scaling.x, scaling.y, scaling.z);

	parent->pos.Set(posate.x, posate.y, posate.z);
	parent->rot = rotate;
	parent->scl.Set(scalete.x, scalete.y, scalete.z);

	parent->local_transform = float4x4::FromTRS(parent->pos, parent->rot, parent->scl);

	// Meshes
	// Mesh import
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		// Check if the mesh is already on the library or needs to be saved
		ResMesh* resmesh = nullptr;
		onlibrary = false;

		aiMesh* ourMesh = scene->mMeshes[node->mMeshes[i]];

		std::map<uint, Resource*> resources = App->resource->resources;

		for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
		{
			if (it->second->Type == Resource::ResType::MESH)
			{
				resmesh = (ResMesh*)it->second;
				if (resmesh->name == node->mName.C_Str())
				{
					// Mesh is already in library
					onlibrary = true;
					LOG("Using mesh %s already loaded on library", resmesh->name.c_str());
					break;
				}
			}
		}

		if (onlibrary == false)
		{
			resmesh = (ResMesh*)App->resource->CreateNewResource(Resource::ResType::MESH);

			resmesh->name = node->mName.C_Str();

			// copy vertices
			resmesh->num_vertex = ourMesh->mNumVertices;
			resmesh->vertex = new float[resmesh->num_vertex * 3];
			memcpy(resmesh->vertex, ourMesh->mVertices, sizeof(float) * resmesh->num_vertex * 3);
			LOG("New mesh with %d vertices", resmesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				resmesh->num_faces = ourMesh->mNumFaces;
				resmesh->num_index = ourMesh->mNumFaces * 3;
				resmesh->index = new uint[resmesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&resmesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (ourMesh->HasNormals())
			{
				resmesh->num_normals = ourMesh->mNumVertices;
				resmesh->normals = new float[resmesh->num_normals * 3];
				memcpy(resmesh->normals, ourMesh->mNormals, sizeof(float) * resmesh->num_normals * 3);
				LOG("New mesh with %d normal", resmesh->num_normals);
				LOG("New mesh with %d idnormal", resmesh->id_normals);
			}
			if (ourMesh->HasTextureCoords(0))
			{
				resmesh->num_tex = ourMesh->mNumVertices;
				resmesh->tex = new float[ourMesh->mNumVertices * 2];

				for (unsigned int i = 0; i < resmesh->num_tex; i++)
				{
					resmesh->tex[i * 2] = ourMesh->mTextureCoords[0][i].x;
					resmesh->tex[i * 2 + 1] = ourMesh->mTextureCoords[0][i].y;

				}
				LOG("New mesh with %d uvs", resmesh->num_tex);
			}

			// Mesh save
			resmesh->SaveResource(resmesh);
		}
		parent->component_list.push_back(resmesh);

		// Textures
		if (ourMesh->HasTextureCoords(0))
		{
			// Material
			if (scene->HasMaterials())
			{
				// Check if the texture is already on the library or needs to be saved
				ResTexture* restexture = nullptr;
				onlibrary = false;

				aiMaterial* texture = nullptr;
				aiString texture_path;

				texture = scene->mMaterials[ourMesh->mMaterialIndex];
				texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

				resources = App->resource->resources;

				for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
				{
					if (it->second->Type == Resource::ResType::TEXTURE)
					{
						restexture = (ResTexture*)it->second;
						if (restexture->texture_path == texture_path.C_Str())
						{
							// Texture is already in library
							onlibrary = true;
							LOG("Using texture %s already loaded on library", restexture->texname.c_str());
							break;
						}
					}
				}

				if (onlibrary == false)
				{
					// The texture is not in library and needs to be saved
					restexture = (ResTexture*)App->resource->CreateNewResource(Resource::ResType::TEXTURE);

					// Default texture
					restexture->defaultex = App->renderer3D->texchec;
					restexture->deftexname = "Checkers";

					// Texture name
					std::string texname;
					std::string texname_2;
					std::string texname_3;

					App->filesys->SplitFilePath(texture_path.C_Str(), &texname, &texname_2, &texname_3);

					restexture->texname = texname_2;
					restexture->texture_path = texture_path.C_Str();
					restexture->assetsFile.append("Assets/Textures/").append(texname_2).append(".").append(texname_3);

					// Texture Import
					uint filesize = 0;
					char* buffer = nullptr;
					restexture->ImportResource(restexture, filesize, buffer);

					// Texture Save
					filesize = restexture->SaveResource(&buffer);

					if (filesize > 0)
					{
						App->filesys->Save(restexture->libraryFile.c_str(), buffer, filesize);
					}
				}
				parent->component_list.push_back(restexture);
			}
		}
	}

	// Resource Model importer (import and save)
	App->serialization->Import_GameObject(parent, id);

	for (int i = 0; i < node->mNumChildren; i++)
	{
		ImportModel(node->mChildren[i], parent, scene, id);
	}
}

void ModuleFBXLoad::ResImport_Texture(const char* assetspath)
{
	// Check if the texture is already on the library or needs to be saved
	ResTexture* restexture = nullptr;
	bool onlibrary = false;

	// Texture path without extension
	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(assetspath, &texname, &texname_2, &texname_3);
	std::string _path;
	_path.append(texname).append(texname_2);

	std::map<uint, Resource*> resources = App->resource->resources;
	resources = App->resource->resources;

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
	{
		if (it->second->Type == Resource::ResType::TEXTURE)
		{
			restexture = (ResTexture*)it->second;
			std::string path;
			path.append("Assets/Textures/").append(restexture->texname);
			
			if (path == _path || path.append(" ") == _path)
			{
				// Texture is already in library
				onlibrary = true;
				App->serialization->textuid = restexture->UID;
				LOG("Using texture %s already loaded on library", restexture->texname.c_str());
				break;
			}
		}
	}

	if (onlibrary == false)
	{
		// The texture is not in library and needs to be saved
		restexture = (ResTexture*)App->resource->CreateNewResource(Resource::ResType::TEXTURE);
		App->serialization->textuid = restexture->UID;

		// Default texture
		restexture->defaultex = App->renderer3D->texchec;
		restexture->deftexname = "Checkers";

		// Texture name
		std::string texname;
		std::string texname_2;
		std::string texname_3;

		App->filesys->SplitFilePath(assetspath, &texname, &texname_2, &texname_3);

		restexture->texname = texname_2;
		restexture->texture_path = assetspath;
		restexture->assetsFile.append("Assets/Textures/").append(texname_2).append(".").append(texname_3);

		// Texture Import 
		uint filesize = 0;
		char* buffer = nullptr;
		restexture->ImportResource(restexture, filesize, buffer);

		// Texture Save
		filesize = restexture->SaveResource(&buffer);

		if (filesize > 0)
		{
			App->filesys->Save(restexture->libraryFile.c_str(), buffer, filesize);
		}
	}
}

void ModuleFBXLoad::Save_Model()
{

}

bool ModuleFBXLoad::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
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