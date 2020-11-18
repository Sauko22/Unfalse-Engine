#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "p2Defs.h"

GameObject::GameObject(GameObject* parent)
{
	parentGameObject = parent;

	if (parentGameObject != nullptr)
	{
		parentGameObject->children_list.push_back(this);
	}
	
	name = " ";
	fbxname = " ";
	pngname = " ";
	index_name = 0;
	normals_name = 0;
	vertex_name = 0;
	faces_name = 0;
	texturescoords_name = 0;
	actualtexgl = 0;

	objSelected = false;
	ObjrenderActive = true;
	Objdelete = false;
	ObjtexActive = false;
	ObjnormActive = false;
	ObjdefauTex = false;
}

GameObject::~GameObject()
{
	for (int i = 0; children_list.size(); i++)
	{
		delete children_list[i];
		children_list[i] = nullptr;
	}
	children_list.clear();
	component_list.clear();
}

void GameObject::update()
{
	// Update components
	for (int i = 0; i < component_list.size(); i++)
	{
		component_list[i]->update();
	}
}

void GameObject::Inspector()
{
	/*ImGui::Checkbox("DeleteObj", &Objdelete);*/
	ImGui::Checkbox("ActiveObj", &ObjrenderActive); ImGui::SameLine();
	ImGui::Text("%s", name.c_str());

	// Update components
	for (int i = 0; i < component_list.size(); i++)
	{
		component_list[i]->inspector();
	}
}

Component* GameObject::AddComponent(Component::compType type)
{
	Component* ret = nullptr;

	switch (type)
	{
	case Component::compType::TRANSFORM:
		ret = new CompTransform(this);
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