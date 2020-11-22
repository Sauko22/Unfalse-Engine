#include "Application.h"
#include "p2Defs.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	UI = new ModuleUI(this);
	primitives = new ModulePrimitives(this);
	fbxload = new ModuleFBXLoad(this);
	filesys = new ModuleFileSystem(this);
	resource = new ModuleResources(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);

	// Primitives creation
	AddModule(primitives);
	
	// Load FBX
	AddModule(fbxload);

	// File System
	AddModule(filesys);
	AddModule(resource);

	// Scenes
	AddModule(scene_intro);

	// Render last
	AddModule(UI);
	AddModule(renderer3D);

	title = TITLE;
	organization = ORGANIZATION;
}

Application::~Application()
{
	for (uint i = 0; i < list_modules.size(); i++)
	{
		RELEASE(list_modules[i])
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	Module* it;

	for (int i = 0; i < list_modules.size() && ret == true; i++) {

		it = list_modules[i];
		ret = it->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (int i = 0; i < list_modules.size() && ret == true; i++) {

		it = list_modules[i];
		ret = it->Start();
	}

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{

}

// ---------------------------------------------
void Application::FinishUpdate()
{

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();


	Module* it;

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

		it = list_modules[i];
		ret = it->PreUpdate();
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

		it = list_modules[i];
		ret = it->Update();
	}

	for (int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++) {

		it = list_modules[i];
		ret = it->PostUpdate();
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	for (uint i = 0; i < list_modules.size(); i++)
	{
		ret = list_modules[i]->CleanUp();
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

const char* Application::GetTitleName() const
{
	return title.c_str();
}

const char* Application::GetOrganizationName() const
{
	return organization.c_str();
}