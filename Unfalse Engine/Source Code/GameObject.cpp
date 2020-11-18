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
	deftexname = " ";
	index_name = 0;
	normals_name = 0;
	vertex_name = 0;
	faces_name = 0;
	texturescoords_name = 0;
	actualtexgl = 0;

	objSelected = false;
	ObjrenderActive = true;
	ObjtexActive = false;
	ObjnormActive = false;
	ObjdefauTex = false;

	texture_h = 0;
	texture_w = 0;
}

GameObject::~GameObject()
{
	for (int i = 0; children_list.size(); i++)
	{
		delete children_list[i];
	}
}

void GameObject::update()
{
	// Update components
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