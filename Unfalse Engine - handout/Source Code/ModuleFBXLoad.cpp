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
	/*glDeleteBuffers(1, &mesh->id_index);
	glDeleteBuffers(1, &mesh->id_vertex);
	glDeleteBuffers(1, &mesh->id_normals);
	glDeleteBuffers(1, &mesh->id_tex);

	delete[] mesh->index;
	delete[] mesh->normals;
	delete[] mesh->vertex;
	delete[] mesh->tex;*/
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
				gameobject->name.append("GameObject_").append(obj);
			}
			else
			{
				gameobject->name = App->input->name;
				gameobject->name.append("_").append(obj);
			}
			LOG("GameObject %s", gameobject->name.c_str());
			Load_Mesh();
			
			/*if (App->renderer3D->j == 0)
			{
				impmesh->meshTexture = App->gameobject->texture1;
			}
			else if (App->renderer3D->j == 1)
			{
				impmesh->meshTexture = App->gameobject->texture2;
			}*/
			
			//mesh->defaultex = App->renderer3D->texchec;

			// Load texture if we have passed the texture path
			compmesh->newmesh->defaultex = App->renderer3D->texchec;

			if (tex_path != nullptr)
			{
				compmesh->newmesh->hastext = true;
				gameobject->AddComponent(Component::compType::MATERIAL);

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
			emptygameobject->name.append("EmptyObject_").append(obj);
		}
		else
		{
			emptygameobject->name = ("%s", App->input->name);
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

/*void EmptyGameObject::RenderEmptyGameObject() const
{
	// Change size
	if (App->fbxload->ResizeFBX == true)
	{
		glScaled(0.05f, 0.05f, 0.05f);
	}
	else
	{
		glScaled(1, 1, 1);
	}
	
	if (gameobject_list.empty() == false)
	{
		for (int i = 0; i < gameobject_list.size(); i++)
		{
			if (gameobject_list[i]->ObjrenderActive == true)
			{
				if (emptytexActive == true)
				{
					if (gameobject_list[i]->ObjtexActive == true)
					{
						// Texture from Devil
						if (gameobject_list[i]->ObjdefauTex == true || emptydefauTex == true)
						{
							glBindTexture(GL_TEXTURE_2D, gameobject_list[i]->defaultex);

						}
						else 
						{
							glBindTexture(GL_TEXTURE_2D, gameobject_list[i]->textgl);
						}
					}
				}
				if (emptytexActive == false && emptydefauTex == true)
				{
					glBindTexture(GL_TEXTURE_2D, gameobject_list[i]->defaultex);
				}
				//Draw Mesh
				glEnableClientState(GL_VERTEX_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, gameobject_list[i]->id_vertex);
				glVertexPointer(3, GL_FLOAT, 0, NULL);

				//Normals
				glEnableClientState(GL_NORMAL_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, gameobject_list[i]->id_normals);
				glNormalPointer(GL_FLOAT, 0, NULL);

				//Uvs
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glBindBuffer(GL_ARRAY_BUFFER, gameobject_list[i]->id_tex);
				glTexCoordPointer(2, GL_FLOAT, 0, NULL);

				glBindBuffer(GL_ARRAY_BUFFER, gameobject_list[i]->id_normals);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gameobject_list[i]->id_index);

				glDrawElements(GL_TRIANGLES, gameobject_list[i]->num_index, GL_UNSIGNED_INT, NULL);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				if (emptytexActive == true)
				{
					if (gameobject_list[i]->ObjtexActive == true)
					{
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}
				if (emptytexActive == false && emptydefauTex == true)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
				}
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);

				if (gameobject_list[i]->ObjnormActive == true)
				{
					glBegin(GL_LINES);
					//glColor3f(1.0f, 0.0f, 0.0f);

					for (size_t k = 0; k < gameobject_list[i]->num_vertex * 3; k += 3)
					{
						GLfloat v_x = gameobject_list[i]->vertex[k];
						GLfloat v_y = gameobject_list[i]->vertex[k + 1];
						GLfloat v_z = gameobject_list[i]->vertex[k + 2];

						GLfloat n_x = gameobject_list[i]->normals[k];
						GLfloat n_y = gameobject_list[i]->normals[k + 1];
						GLfloat n_z = gameobject_list[i]->normals[k + 2];

						glVertex3f(v_x, v_y, v_z);
						glVertex3f(v_x + n_x, v_y + n_y, v_z + n_z);
					}
					glEnd();
				}
			}
		}
	}
}*/

void ModuleFBXLoad::LoadTexture(char* file_path) 
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	compmesh->newmesh->textgl = ilutGLBindTexImage();

	ilDeleteImages(1, &textIL);
}

/*void ModuleFBXLoad::LoadTextureObject(char* file_path, int k, int i)
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	App->gameobject->emptygameobject_list[i]->gameobject_list[k]->textgl = ilutGLBindTexImage();

	ilDeleteImages(1, &textIL);
}*/

