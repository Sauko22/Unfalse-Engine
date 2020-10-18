#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleFBXLoad.h"


#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#pragma comment (lib, "Assimp/libx86/assimp.lib")



ModuleFBXLoad::ModuleFBXLoad(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

// Destructor
ModuleFBXLoad::~ModuleFBXLoad()
{}

// Called before render is available
bool ModuleFBXLoad::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	Import("warrior.FBX");
	
	return ret;
}





// Called before quitting
bool ModuleFBXLoad::CleanUp()
{
	LOG("Destroying 3D Renderer");

	aiDetachAllLogStreams();

	return true;
}

// PostUpdate present buffer to screen
void ModuleFBXLoad::Import(char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* ourMesh = scene->mMeshes[i];

			// copy vertices
			impmesh.num_vertex = ourMesh->mNumVertices;
			impmesh.vertex = new float[impmesh.num_vertex * 3];
			memcpy(impmesh.vertex, ourMesh->mVertices, sizeof(float) * impmesh.num_vertex * 3);
			LOG("New mesh with %d vertices", impmesh.num_vertex);

			// copy faces
			if (ourMesh->HasFaces())
			{
				impmesh.num_index = ourMesh->mNumFaces * 3;
				impmesh.index = new uint[impmesh.num_index]; // assume each face is a triangle
				for (uint i = 0; i < ourMesh->mNumFaces; ++i)
				{
					if (ourMesh->mFaces[i].mNumIndices != 3) { LOG("WARNING, geometry face with != 3 indices!"); }

					else { memcpy(&impmesh.index[i * 3], ourMesh->mFaces[i].mIndices, 3 * sizeof(uint)); }
				}
			}
		}
		LOG("%s Loaded", file_path);
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
	}
}