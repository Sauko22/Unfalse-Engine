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

	

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleFBXLoad::PreUpdate()
{
	

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleFBXLoad::PostUpdate()
{

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleFBXLoad::CleanUp()
{
	LOG("Destroying 3D Renderer");

	

	return true;
}


