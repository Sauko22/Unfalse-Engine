#include "Globals.h"
#include "Application.h"
#include "ModuleGameObject.h"
#include "p2Defs.h"

ModuleGameObject::ModuleGameObject(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	texture1 = nullptr;
	texture2 = nullptr;
}

ModuleGameObject::~ModuleGameObject()
{}

bool ModuleGameObject::Start()
{
	LOG("Loading GameObject");
	bool ret = true;

	texture1 = "E:\\Github Repositories\\Unfalse-Engine\\Unfalse Engine - handout\\Game\\Assets\\Baker_house\\Baker_house.png";
	texture2 = "E:\\Github Repositories\\Unfalse-Engine\\Unfalse Engine - handout\\Game\\Assets\\Blitzcrank\\blitzcrank_skin11_TX_CM.png";

	return ret;
}

bool ModuleGameObject::CleanUp()
{
	LOG("Unloading GameObject");

	for (uint i = 0; i < gameobject_list.size(); i++)
	{
		RELEASE(gameobject_list[i])
	}

	return true;
}

update_status ModuleGameObject::Update()
{
	bool ret = true;

	if (ImGui::Begin("Inspector", NULL))
	{
		if (App->gameobject->gameobject_list.empty() == false)
		{
			for (int i = 0; i < App->gameobject->gameobject_list.size(); i++)
			{
				if (App->gameobject->gameobject_list[i]->objSelected == true)
				{
					App->gameobject->gameobject_list[i]->showInspectorWin();
				}
			}
		}
		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

void GameObject::CreateGameObject()
{
	if (App->gameobject->temp_comp_list.empty() == false)
	{
		for (int i = 0; i < App->gameobject->temp_comp_list.size(); i++)
		{
			comp_list.push_back(App->gameobject->temp_comp_list[i]);
		}
	}

	/*if (App->renderer3D->j == 0)
	{
		App->fbxload->gameobject->objectTexture = "E:\\Github Repositories\\Unfalse-Engine\\Unfalse Engine - handout\\Game\\Assets\\Baker_house\\Baker_house.png";
	}
	else if (App->renderer3D->j == 1)
	{
		App->fbxload->gameobject->objectTexture = "E:\\Github Repositories\\Unfalse-Engine\\Unfalse Engine - handout\\Game\\Assets\\Blitzcrank\\blitzcrank_skin11_TX_CM.png";
	}*/
	App->gameobject->gameobject_list.push_back(App->fbxload->gameobject);
}

