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
}

GameObject::~GameObject()
{
	for (int i = 0; i < component_list.size(); i++)
	{
		if (component_list[i] != nullptr)
		{
			delete component_list[i];
		}
	}
	component_list.clear();
}

void GameObject::update()
{
	// Update components
	for (int i = 0; i < component_list.size(); i++)
	{
		component_list[i]->update();
	}
	UpdateAABB();
}

void GameObject::Inspector()
{
	ImGui::Checkbox("DeleteObj", &Objdelete);
	ImGui::Checkbox("ActiveObj", &ObjrenderActive); ImGui::SameLine();
	ImGui::Text("%s", name.c_str());

	// Update components
	for (int i = 0; i < component_list.size(); i++)
	{
		component_list[i]->inspector();
	}
}

const AABB& GameObject::GetAABB() const
{
	return aabb;
}

const OBB& GameObject::GetOBB() const
{
	return obb;
}

void GameObject::UpdateAABB()
{
	CompMesh* mesh = (CompMesh*)GetComponent(Component::compType::MESH);
	CompTransform* transform = (CompTransform*)GetComponent(Component::compType::TRANSFORM);
	
	if (mesh != nullptr)
	{
		App->fbxload->GenerateAABB(mesh);

		obb = mesh->GetAABB();
		obb.Transform(transform->local_transform);

		aabb.SetNegativeInfinity();
		aabb.Enclose(obb);

		mesh->bbox = aabb;

		if (App->primitives->bbox_list.empty() == false)
		{
			for (int i = 0; i < App->primitives->bbox_list.size(); i++)
			{
				App->primitives->bbox_list[i]->bbox_delete = true;
			}
		}

		App->fbxload->GenerateLines(mesh);
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

Component* GameObject::GetComponent(Component::compType type)
{
	Component* ret = nullptr;

	for (int i = 0; i < component_list.size(); i++)
	{
		if (type == component_list[i]->type)
			return component_list[i];
	}

	return ret;
}