#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ModulePrimitives.h"
#include "ModuleFBXLoad.h"
#include "ModuleGameObject.h"
#include "ModuleFileSystem.h"

#include <string>
#include <vector>
#include <list>

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleUI* UI;
	ModulePrimitives* primitives;
	ModuleFBXLoad* fbxload;
	ModuleGameObject* gameobject;
	ModuleFileSystem* filesys;

private:

	std::vector<Module*> list_modules;

	std::string title;
	std::string organization;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	const char* GetOrganizationName() const;
	const char* GetTitleName() const;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;