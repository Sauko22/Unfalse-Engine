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

	for (uint i = 0; i < emptygameobject_list.size(); i++)
	{
		RELEASE(emptygameobject_list[i])
	}

	return true;
}

update_status ModuleGameObject::Update()
{
	bool ret = true;

	if (ImGui::Begin("Inspector", NULL))
	{
		if (App->gameobject->emptygameobject_list.empty() == false)
		{
			for (int i = 0; i < App->gameobject->emptygameobject_list.size(); i++)
			{
				if (App->gameobject->emptygameobject_list[i]->emptySelected == true)
				{
					App->gameobject->emptygameobject_list[i]->showEmptyInspectorWin();

					if (App->input->texture_dropped == true)
					{
						for (int k = 0; k < App->gameobject->emptygameobject_list[i]->gameobject_list.size(); k++)
						{
							//App->fbxload->LoadTextureObject(App->input->dropped_filedir, k, i);
						}
						App->input->texture_dropped = false;
					}
				}

				for (int j = 0; j < App->gameobject->emptygameobject_list[i]->gameobject_list.size(); j++)
				{
					if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->objSelected == true)
					{
						if (App->input->texture_obj_dropped == true)
						{
							if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->objSelected == true)
							{
								//App->fbxload->LoadTextureObject(App->input->dropped_filedir, j, i);
							}

							App->input->texture_obj_dropped = false;
						}
					}

					if (App->gameobject->emptygameobject_list[i]->gameobject_list[j]->objSelected == true)
					{
						App->gameobject->emptygameobject_list[i]->gameobject_list[j]->showInspectorWin();
					}
				}
			}
		}
		ImGui::End();
	}

	return UPDATE_CONTINUE;
}

void EmptyGameObject::CreateEmptyGameObject()
{
	if (App->gameobject->temp_gameobj_list.empty() == false)
	{
		for (int i = 0; i < App->gameobject->temp_gameobj_list.size(); i++)
		{
			gameobject_list.push_back(App->gameobject->temp_gameobj_list[i]);
		}
	}
	App->gameobject->emptygameobject_list.push_back(App->fbxload->emptygameobject);
}

GameObject::GameObject()
{}

GameObject::~GameObject()
{}

void GameObject::update()
{
	// Update components
	/*std::vector<Component*>::iterator it;
	for (it = component_list.begin(); it != component_list.end(); ++it)
	{
		(*it)->update();
	}*/
	for (int i = 0; i < component_list.size(); i++)
	{
		component_list[i]->update();
	}
}

Component* GameObject::AddComponent(Component::compType type)
{
	Component* ret = nullptr;

	switch (type)
	{
	case Component::compType::TRANSFORM:
		ret = new CompTransform(this, vec3(0, 0, 0), vec3(0, 0, 0), vec3(0, 0, 0));
		break;
	case Component::compType::MESH: 
		ret = new CompMesh(this); 
		break;
	case Component::compType::MATERIAL:
		ret = new CompMaterial(this);
		break;
	}

	component_list.push_back(ret);

	return ret;
}


