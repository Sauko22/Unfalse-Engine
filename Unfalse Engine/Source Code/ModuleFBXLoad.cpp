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

#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

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

	/*std::string obj = std::to_string(j);

	pgameobject->name.append("GameObject").append(obj);*/

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

	comptrans = (CompTransform*)pgameobject->AddComponent(Component::compType::TRANSFORM);

	aiVector3D translation, scaling;
	aiQuaternion rotation;
	float3 pos, scale;
	Quat rot; 

	node->mTransformation.Decompose(scaling, rotation, translation);

	pos.Set(translation.x, translation.y, translation.z);
	rot.Set(rotation.x, rotation.y, rotation.z, rotation.w);
	scale.Set(scaling.x, scaling.y, scaling.z);

	comptrans->pos.Set(pos.x, pos.y, pos.z);
	comptrans->rot = rot;
	comptrans->scl.Set(scale.x, scale.y, scale.z);
	comptrans->transform = float4x4::FromTRS(pos, rot, scale);
	//comptrans->transform.Transpose();

	LOG("Position: %f, %f, %f", comptrans->pos.x, comptrans->pos.y, comptrans->pos.z);
	LOG("Rotation: %f, %f, %f, %f", comptrans->rot.x, comptrans->rot.y, comptrans->rot.z, comptrans->rot.w);
	LOG("Scale: %f, %f, %f", comptrans->scl.x, comptrans->scl.y, comptrans->scl.z);

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

		Load_Texture(ourMesh, scene, pgameobject);

		LOG("Mesh loaded");
		LOG("Components: %i", pgameobject->component_list.size());
	}
	
	for (int i = 0; i < node->mNumChildren; i++)
	{
		Import(node->mChildren[i], pgameobject, scene);
	}
}

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

void ModuleFBXLoad::Load_Texture(aiMesh* ourMesh, const aiScene* scene, GameObject* gameobject)
{
	// Load texture if we have passed the texture path
	compmesh->defaultex = App->renderer3D->texchec;
	compmesh->deftexname = "Checkers";

	char* tex_path = nullptr;

	if (ourMesh->HasTextureCoords(0))
	{
		// Material
		if (scene->HasMaterials())
		{
			aiMaterial* texture = nullptr;
			aiString texture_path;
			std::string texname;
			std::string texname_2;
			std::string texname_3;

			texture = scene->mMaterials[ourMesh->mMaterialIndex];

			//aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, ourMesh->mMaterialIndex, &texture_path);
			texture->GetTexture(aiTextureType_DIFFUSE, 0, &texture_path);

			App->filesys->SplitFilePath(texture_path.C_Str(), &texname, &texname_2, &texname_3);

			compmesh->texname = texname_2;

			texturepath = "Assets/Textures/";
			texturepath.append(texname_2).append(".").append(texname_3);

			tex_path = (char*)texturepath.c_str();
			
			LOG("Texture path: %s", tex_path);
		}

		if (tex_path != nullptr && texturepath != "Assets/Textures/.")
		{
			compmesh->hastext = true;
			gameobject->AddComponent(Component::compType::MATERIAL);
			gameobject->ObjtexActive = true;
			compmesh->texactive = true;

			gameobject->pngname = tex_path;

			LoadTexture(tex_path, gameobject);
			LOG("Texture from import %s Loaded", tex_path);
		}
	}
}




void ModuleFBXLoad::LoadTexture(char* file_path, GameObject* gameobject) 
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	compmesh->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	compmesh->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	compmesh->textgl = ilutGLBindTexImage();

	gameobject->actualtexgl = compmesh->textgl;
	
	ilDeleteImages(1, &textIL);
}

void ModuleFBXLoad::LoadTextureObject(char* file_path, GameObject* gameobject)
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	std::string texname;
	std::string texname_2;
	std::string texname_3;

	App->filesys->SplitFilePath(file_path, &texname, &texname_2, &texname_3);

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

