#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleFBXLoad.h"
#include "GameObject.h"

#include "Glew\include\glew.h"
#pragma comment (lib, "Glew/libx86/glew32.lib") /* link Microsoft OpenGL lib   */

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

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


ModuleFBXLoad::ModuleFBXLoad(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	/*gameobject = nullptr;*/
	compmesh = nullptr;
	comptrans = nullptr;
	j = 0;
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
		Import(scene->mRootNode, parent, scene);
		j++;
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}

// PostUpdate present buffer to screen
void ModuleFBXLoad::Import(aiNode* node, GameObject* parent, const aiScene* scene)
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

	comptrans->pos.Set(posate.x, posate.y, posate.z);
	comptrans->rot = rotate;
	comptrans->scl.Set(scalete.x, scalete.y, scalete.z);

	comptrans->local_transform = float4x4::FromTRS(comptrans->pos, comptrans->rot, comptrans->scl);
	//comptrans->local_transform.Transpose();

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		compmesh = (CompMesh*)pgameobject->AddComponent(Component::compType::MESH);
		
		aiMesh* ourMesh = scene->mMeshes[node->mMeshes[i]];

		compmesh->name = node->mName.C_Str();

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

		//ImportMesh(compmesh);
		//Save_Mesh(compmesh);

		// Texture importer
		Import_Texture(ourMesh, scene, pgameobject, compmesh);
		
		LOG("Mesh loaded");
		LOG("Components: %i", pgameobject->component_list.size());
	}
	
	for (int i = 0; i < node->mNumChildren; i++)
	{
		Import(node->mChildren[i], pgameobject, scene);
	}
}

// Mesh importer

//void ModuleFBXLoad::ImportMesh(CompMesh* mesh)
//{
//	//Save_Mesh(mesh);
//
//	//LoadMesh(mesh);
//
//	// Save Mesh //////////////////////////////////
//	// amount of indices / vertices / colors / normals / texture_coords / AABB
//	uint ranges[2] = { mesh->num_index, mesh->num_vertex };
//	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3;
//	char* fileBuffer = new char[size]; // Allocate
//	char* cursor = fileBuffer;
//	uint bytes = sizeof(ranges); // First store ranges
//	memcpy(cursor, ranges, bytes);
//	cursor += bytes;
//	/*std::string texname;
//	std::string texname_2;
//	std::string texname_3;*/
//	std::string path = "";
//
//	// Store indices
//	bytes = sizeof(uint) * mesh->num_index;
//	memcpy(cursor, mesh->index, bytes);
//	cursor += bytes;
//
//	// Store vertex
//	bytes = sizeof(float) * mesh->num_vertex * 3;
//	memcpy(cursor, mesh->vertex, bytes);
//	cursor += bytes;
//
//	// Store normals
//	bytes = sizeof(float) * mesh->num_normals * 3;
//	memcpy(cursor, mesh->normals, bytes);
//	cursor += bytes;
//
//	// Store textures
//	bytes = sizeof(float) * mesh->num_tex;
//	memcpy(cursor, mesh->tex, bytes);
//
//	//App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
//	path.append("Library/Meshes/Baker_house")/*.append(texname_2)*/;
//
//	App->filesys->Save(path.c_str(), fileBuffer, size);
//	//////////////////////////////////////////////
//
//	// Load Mesh /////////////////////////////////
//	/*file_path = "";*/
//	uint64 filesize = 0;
//	char* buffer = nullptr;
//
//	// The mesh is in the library
//	/*file_path.append("Library/Meshes/").append("Baker_house");*/
//	filesize = App->filesys->Load(path.c_str(), &buffer);
//
//	// The mesh is not in the library
//	/*if (filesize == 0)
//	{
//		file_path = "";
//		file_path.append("Assets/Models/").append("BakerHouse").append(".").append("fbx");
//		filesize = App->filesys->Load(path.c_str(), &buffer);
//	}*/
//
//	char* _cursor = buffer;
//	// amount of indices / vertices / colors / normals / texture_coords
//	uint _ranges[5];
//	uint _bytes = sizeof(_ranges);
//	memcpy(_ranges, _cursor, _bytes);
//	_cursor += _bytes;
//
//	mesh->num_index = _ranges[0];
//	mesh->num_vertex = _ranges[1];
//	mesh->num_normals = _ranges[2];
//	mesh->num_tex = _ranges[3];
//
//	// Load indices
//	_bytes = sizeof(uint) * mesh->num_index;
//	mesh->index = new uint[mesh->num_index];
//	memcpy(mesh->index, _cursor, _bytes);
//	_cursor += _bytes;
//
//	// Load vertexs
//	_bytes = sizeof(float) * mesh->num_vertex * 3;
//	mesh->vertex = new float[mesh->num_vertex * 3];
//	memcpy(mesh->vertex, _cursor, _bytes);
//	_cursor += _bytes;
//
//	// Load normals
//	_bytes = sizeof(float) * mesh->num_normals * 3;
//	mesh->normals = new float[mesh->num_normals * 3];
//	memcpy(mesh->normals, _cursor, _bytes);
//	_cursor += _bytes;
//
//	// Load textures
//	_bytes = sizeof(float) * mesh->num_tex * 3;
//	mesh->tex = new float[mesh->num_tex * 3];
//	memcpy(mesh->tex, _cursor, _bytes);
//	/////////////////////////////////////////////
//}
//
//void ModuleFBXLoad::LoadMesh(CompMesh* mesh)
//{
//	/*char* cursor = buffer;
//	// amount of indices / vertices / colors / normals / texture_coords
//	uint ranges[5];
//	uint bytes = sizeof(ranges);
//	memcpy(ranges, cursor, bytes);
//	cursor += bytes;
//	mesh->num_indices = ranges[0];
//	mesh->num_vertices = ranges[1];
//	// Load indices
//	bytes = sizeof(uint) * resource->num_indices;
//	resource->indices = new uint[resource->num_indices];
//	memcpy(resource->indices, cursor, bytes);
//	cursor += bytes;*/
//}

void ModuleFBXLoad::Load_Mesh()
{
	//Vertex of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, compmesh->id_vertex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * compmesh->num_vertex * 3, &compmesh->vertex[0], GL_STATIC_DRAW);

	//Normal faces of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->num_normals * 3, &compmesh->normals[0], GL_STATIC_DRAW);

	//Indices of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->id_index);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_index);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * compmesh->num_index, &compmesh->index[0], GL_STATIC_DRAW);

	//Uvs of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->id_tex);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->num_tex * 2, &compmesh->tex[0], GL_STATIC_DRAW);
}

void ModuleFBXLoad::Save_Mesh(CompMesh* mesh)
{
	// amount of indices / vertices / colors / normals / texture_coords / AABB
	uint ranges[2] = { mesh->num_index, mesh->num_vertex };
	uint size = sizeof(ranges) + sizeof(uint) * mesh->num_index + sizeof(float) * mesh->num_vertex * 3;
	char* fileBuffer = new char[size]; // Allocate
	char* cursor = fileBuffer;
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);
	cursor += bytes;
	std::string texname;
	std::string texname_2;
	std::string texname_3;
	std::string path;
	
	// Store indices
	bytes = sizeof(uint) * mesh->num_index;
	memcpy(cursor, mesh->index, bytes);
	cursor += bytes;

	// Store vertex
	bytes = sizeof(float) * mesh->num_vertex;
	memcpy(cursor, mesh->vertex, bytes);
	cursor += bytes;

	// Store normals
	bytes = sizeof(float) * mesh->num_normals;
	memcpy(cursor, mesh->normals, bytes);
	cursor += bytes;

	// Store textures
	bytes = sizeof(float) * mesh->num_tex;
	memcpy(cursor, mesh->tex, bytes);

	App->filesys->SplitFilePath(file_path.c_str(), &texname, &texname_2, &texname_3);
	path.append("Library/Meshes/").append(texname_2);

	App->filesys->Save(path.c_str(), fileBuffer, size);
}

void ModuleFBXLoad::Import_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject, CompMesh* compmesh)
{
	// Default texture
	compmesh->defaultex = App->renderer3D->texchec;
	compmesh->deftexname = "Checkers";

	if (ourMesh->HasTextureCoords(0))
	{
		// Material
		if (scene->HasMaterials())
		{
			// Active texture
			compmesh->hastext = true;
			gameobject->AddComponent(Component::compType::MATERIAL);
			gameobject->ObjtexActive = true;
			compmesh->texactive = true;

			// Texture name
			aiMaterial* texture = nullptr;
			aiString texture_path;
			std::string texname;
			std::string texname_2;
			std::string texname_3;

			texture = scene->mMaterials[ourMesh->mMaterialIndex];
			texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

			App->filesys->SplitFilePath(texture_path.C_Str(), &texname, &texname_2, &texname_3);

			compmesh->texname = texname_2;
			
			// Texture Import 
			ImportTexture(texname_2, texname_3);
			

			// Texture save
			uint filesize = 0;
			char* buffer = nullptr;
			filesize = SaveTexture(ourMesh, &buffer);

			if (filesize > 0)
			{
				file_path = "";
				file_path.append("Library/Textures/").append(texname_2);
				App->filesys->Save(file_path.c_str(), buffer, filesize);
			}

			// Texture load
			LoadTexture(buffer, filesize, gameobject);
		}
	}
}

void ModuleFBXLoad::ImportTexture(std::string texname_2, std::string texname_3)
{
	file_path = "";
	uint64 filesize = 0;
	char* buffer = nullptr;

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
	}

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, filesize);
}

void ModuleFBXLoad::LoadTexture(char* buffer, uint filesize, GameObject* gameobject) 
{
	ILuint textIL;

	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadL(IL_DDS, (const void*)buffer, filesize);

	compmesh->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	compmesh->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	compmesh->textgl = ilutGLBindTexImage();

	gameobject->actualtexgl = compmesh->textgl;
	
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

void ModuleFBXLoad::LoadTextureObject(char* buffer, uint filesize, GameObject* gameobject, char* name)
{
	ILuint textIL;

	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadL(IL_TYPE_UNKNOWN, (const void*)buffer, filesize);

	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(name, &texname, &texname_2, &texname_3);

	compmesh->texname = texname_2;

	for (int i = 0; i < gameobject->component_list.size(); i++)
	{
		gameobject->component_list[i]->newtexgl = ilutGLBindTexImage();
		gameobject->component_list[i]->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
		gameobject->component_list[i]->texture_w = ilGetInteger(IL_IMAGE_WIDTH);
		gameobject->component_list[i]->texname = texname_2;
	}

	ilDeleteImages(1, &textIL);
}

