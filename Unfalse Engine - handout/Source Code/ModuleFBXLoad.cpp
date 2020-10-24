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
	impmesh = new Mesh();
	mesh = nullptr;
}

// Destructor
ModuleFBXLoad::~ModuleFBXLoad()
{
	glDeleteBuffers(1, &impmesh->id_index);
	glDeleteBuffers(1, &impmesh->id_vertex);
	glDeleteBuffers(1, &impmesh->id_normals);
	glDeleteBuffers(1, &impmesh->id_tex);

	delete[] impmesh->index;
	delete[] impmesh->normals;
	delete[] impmesh->vertex;
	delete[] impmesh->tex;
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
void ModuleFBXLoad::Import(char* file_path, int texID)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* ourMesh = scene->mMeshes[i];

			// copy vertices
			impmesh->num_vertex = ourMesh->mNumVertices;
			impmesh->vertex = new float[impmesh->num_vertex * 3];
			memcpy(impmesh->vertex, ourMesh->mVertices, sizeof(float) * impmesh->num_vertex * 3);
			LOG("New mesh with %d vertices", impmesh->num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				impmesh->num_index = ourMesh->mNumFaces * 3;
				impmesh->index = new uint[impmesh->num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&impmesh->index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			if (ourMesh->HasNormals()) {

				impmesh->num_normals = ourMesh->mNumVertices;
				impmesh->normals = new float[impmesh->num_normals * 3];
				memcpy(impmesh->normals, ourMesh->mNormals, sizeof(float) * impmesh->num_normals * 3);
				LOG("New mesh with %d normal", impmesh->num_normals);
				LOG("New mesh with %d idnormal", impmesh->id_normals);

			}
			if (ourMesh->HasTextureCoords(0)) {
				impmesh->num_tex = ourMesh->mNumVertices;
				impmesh->tex = new float[ourMesh->mNumVertices * 2];

				for (unsigned int i= 0; i < impmesh->num_tex; i++)
				{
					impmesh->tex[i * 2] = ourMesh->mTextureCoords[0][i].x;
					impmesh->tex[i * 2 + 1] = ourMesh->mTextureCoords[0][i].y;

				}
				impmesh->imgID = texID;
				LOG("New mesh with %d uvs", impmesh->num_tex);
			}
		}
		aiReleaseImport(scene);
		Load_Mesh();
		
		LOG("%s Loaded", file_path);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}

void ModuleFBXLoad::Load_Mesh()
{
	// Our mesh
	mesh = impmesh;

	//Vertex of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_vertex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh->num_vertex * 3, &mesh->vertex[0], GL_STATIC_DRAW);

	//Normal faces of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_normals * 3, &mesh->normals[0], GL_STATIC_DRAW);

	//Indices of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_index);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_index);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * mesh->num_index, &mesh->index[0], GL_STATIC_DRAW);

	//Uvs of the mesh
	glGenBuffers(1, (GLuint*)&mesh->id_tex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->num_tex * 2, &mesh->tex[0], GL_STATIC_DRAW);
}

void ModuleFBXLoad::Draw_Mesh()
{
	
	glEnable(GL_TEXTURE_2D);
	// Texture from Devil
	
	glBindTexture(GL_TEXTURE_2D, textgl);

	//Draw Mesh
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, impmesh->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//Normals
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, impmesh->id_normals);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//Uvs
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, impmesh->id_tex);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, impmesh->id_normals);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, impmesh->id_index);

	glDrawElements(GL_TRIANGLES, impmesh->num_index, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void ModuleFBXLoad::LoadTexture(char* file_path) 
{
	ilGenImages(1, &textIL);
	ilBindImage(textIL);

	ilLoadImage(file_path);

	textgl = ilutGLBindTexImage();

	ilDeleteImages(1, &textIL);
}