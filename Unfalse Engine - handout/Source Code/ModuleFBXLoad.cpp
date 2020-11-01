#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleFBXLoad.h"

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




ModuleFBXLoad::ModuleFBXLoad(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	emptygameobject = nullptr;
	gameobject = nullptr;
	compmesh = nullptr;
	ResizeFBX = false;
	j = 0;
}

// Destructor
ModuleFBXLoad::~ModuleFBXLoad()
{
	glDeleteBuffers(1, &compmesh->newmesh->id_index);
	glDeleteBuffers(1, &compmesh->newmesh->id_vertex);
	glDeleteBuffers(1, &compmesh->newmesh->id_normals);
	glDeleteBuffers(1, &compmesh->newmesh->id_tex);

	delete[] compmesh->newmesh->index;
	delete[] compmesh->newmesh->normals;
	delete[] compmesh->newmesh->vertex;
	delete[] compmesh->newmesh->tex;
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

// PostUpdate present buffer to screen
void ModuleFBXLoad::Import(char* file_path, uint filesize, char* tex_path)
{
	const aiScene* scene = aiImportFileFromMemory(file_path, filesize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);
	//const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		emptygameobject = new EmptyGameObject;

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			gameobject = new GameObject();
			gameobject->AddComponent(Component::compType::TRANSFORM);
			compmesh = dynamic_cast<CompMesh*>(gameobject->AddComponent(Component::compType::MESH));
			compmesh->newmesh = new Mesh;

			aiMesh* ourMesh = scene->mMeshes[i];

			// copy vertices
			compmesh->newmesh->num_vertex = ourMesh->mNumVertices;
			compmesh->newmesh->vertex = new float[compmesh->newmesh->num_vertex * 3];
			memcpy(compmesh->newmesh->vertex, ourMesh->mVertices, sizeof(float) * compmesh->newmesh->num_vertex * 3);
			LOG("New mesh with %d vertices", compmesh->newmesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				compmesh->newmesh->num_faces = ourMesh->mNumFaces;
				compmesh->newmesh->num_index = ourMesh->mNumFaces * 3;
				compmesh->newmesh->index = new uint[compmesh->newmesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&compmesh->newmesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (ourMesh->HasNormals()) 
			{
				compmesh->newmesh->num_normals = ourMesh->mNumVertices;
				compmesh->newmesh->normals = new float[compmesh->newmesh->num_normals * 3];
				memcpy(compmesh->newmesh->normals, ourMesh->mNormals, sizeof(float) * compmesh->newmesh->num_normals * 3);
				LOG("New mesh with %d normal", compmesh->newmesh->num_normals);
				LOG("New mesh with %d idnormal", compmesh->newmesh->id_normals);
			}
			if (ourMesh->HasTextureCoords(0)) 
			{
				compmesh->newmesh->num_tex = ourMesh->mNumVertices;
				compmesh->newmesh->tex = new float[ourMesh->mNumVertices * 2];

				for (unsigned int i= 0; i < compmesh->newmesh->num_tex; i++)
				{
					compmesh->newmesh->tex[i * 2] = ourMesh->mTextureCoords[0][i].x;
					compmesh->newmesh->tex[i * 2 + 1] = ourMesh->mTextureCoords[0][i].y;

				}
				LOG("New mesh with %d uvs", compmesh->newmesh->num_tex);
			}

			std::string obj = std::to_string(i);
			if (App->input->name == "")
			{
				gameobject->name.append("BakerHouse_").append(obj);
				gameobject->fbxname.append("BakerHouse").append(".fbx");
				
			}
			else
			{
				gameobject->name = App->input->name;
				gameobject->name.append("_").append(obj);
				gameobject->fbxname = App->input->name;
				gameobject->fbxname.append(".fbx");
			}
			if (App->UI->cube == true)
			{
				std::string cub = ("Cube_");
				gameobject->name = cub.append(obj);
				gameobject->fbxname = cub.append(".fbx");
			}
			else if (App->UI->cylinder == true)
			{
				std::string cyl = ("Cylinder_");
				gameobject->name = cyl.append(obj);
				gameobject->fbxname = cyl.append(".fbx");
			}
			else if (App->UI->sphere == true)
			{
				std::string sph = ("Sphere_");
				gameobject->name = sph.append(obj);
				gameobject->fbxname = sph.append(".fbx");
			}
			else if (App->UI->pyramid == true)
			{
				std::string pyr = ("Pyramid_");
				gameobject->name = pyr.append(obj);
				gameobject->fbxname = pyr.append(".fbx");
			}
			LOG("GameObject %s", gameobject->name.c_str());
			
			gameobject->faces_name = compmesh->newmesh->num_faces;
			gameobject->texturescoords_name = compmesh->newmesh->num_tex;
			gameobject->normals_name = compmesh->newmesh->num_normals;
			gameobject->vertex_name = compmesh->newmesh->num_vertex;
			gameobject->index_name = compmesh->newmesh->num_index;
			
			Load_Mesh();
			
			// Load texture if we have passed the texture path
			compmesh->newmesh->defaultex = App->renderer3D->texchec;
			gameobject->deftexname = "Checkers";

			// Material
			if (scene->HasMaterials())
			{
				aiMaterial* texture = nullptr;
				aiString texture_path;

				texture = scene->mMaterials[ourMesh->mMaterialIndex];

				aiGetMaterialTexture(texture, aiTextureType_DIFFUSE, ourMesh->mMaterialIndex, &texture_path);

				std::string texturepath = "Assets/Textures/";
				texturepath.append(texture_path.C_Str());
				if (texturepath != "Assets/Textures/")
				{
					tex_path = (char*)texturepath.c_str();
					LOG("%s", tex_path);
				}
			}
			if (tex_path != nullptr)
			{
				compmesh->newmesh->hastext = true;
				gameobject->AddComponent(Component::compType::MATERIAL);
				gameobject->ObjtexActive = true;

				gameobject->pngname = tex_path;

				LoadTexture(tex_path);
				LOG("Texture from import Loaded");
			}

			LOG("Gameobject Components: %i", gameobject->component_list.size());

			compmesh->mesh_list.push_back(compmesh->newmesh);
			
			App->gameobject->temp_gameobj_list.push_back(gameobject);
			emptygameobject->gameObjects++;
		}
		j++;
		std::string obj = std::to_string(j);
		if (App->input->name == "")
		{
			emptygameobject->name.append("BakerHouse_").append(obj);
		}
		else
		{
			emptygameobject->name = ("%s", App->input->name);
		}
		if (App->UI->cube == true)
		{
			emptygameobject->name = "Cube";
			App->UI->cube = false;
		}
		else if (App->UI->sphere == true)
		{
			emptygameobject->name = "Sphere";
			App->UI->sphere = false;
		}
		else if (App->UI->pyramid == true)
		{
			emptygameobject->name = "Pyramid";
			App->UI->pyramid = false;
		}
		else if (App->UI->cylinder == true)
		{
			emptygameobject->name = "Cylinder";
			App->UI->cylinder = false;
		}
		LOG("EmptyObject %s", emptygameobject->name.c_str());

		emptygameobject->CreateEmptyGameObject();
		
		App->gameobject->temp_gameobj_list.clear();

		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}

void ModuleFBXLoad::Load_Mesh()
{
	//Vertex of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->newmesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, compmesh->newmesh->id_vertex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * compmesh->newmesh->num_vertex * 3, &compmesh->newmesh->vertex[0], GL_STATIC_DRAW);

	//Normal faces of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->newmesh->id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->newmesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->newmesh->num_normals * 3, &compmesh->newmesh->normals[0], GL_STATIC_DRAW);

	//Indices of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->newmesh->id_index);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->newmesh->id_index);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * compmesh->newmesh->num_index, &compmesh->newmesh->index[0], GL_STATIC_DRAW);

	//Uvs of the mesh
	glGenBuffers(1, (GLuint*)&compmesh->newmesh->id_tex);
	glBindBuffer(GL_ARRAY_BUFFER, compmesh->newmesh->id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * compmesh->newmesh->num_tex * 2, &compmesh->newmesh->tex[0], GL_STATIC_DRAW);
}



void ModuleFBXLoad::LoadTexture(char* file_path) 
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	gameobject->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	gameobject->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	compmesh->newmesh->textgl = ilutGLBindTexImage();

	gameobject->actualtexgl = compmesh->newmesh->textgl;

	ilDeleteImages(1, &textIL);
}

void ModuleFBXLoad::LoadTextureObject(char* file_path, int i, int k, int j)
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	App->gameobject->emptygameobject_list[i]->gameobject_list[k]->texture_h = ilGetInteger(IL_IMAGE_HEIGHT);
	App->gameobject->emptygameobject_list[i]->gameobject_list[k]->texture_w = ilGetInteger(IL_IMAGE_WIDTH);

	App->gameobject->emptygameobject_list[i]->gameobject_list[k]->component_list[j]->newtexgl = ilutGLBindTexImage();

	ilDeleteImages(1, &textIL);
}

