#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>
#include "Component.h"

typedef unsigned int GLuint;

class GameObject
{
public:
	GameObject(GameObject* parent);
	~GameObject();

	std::vector<GameObject*> children_list;
	std::vector<Component*> component_list;

	void update();
	void Inspector();

	Component* AddComponent(Component::compType type);

	GameObject* parentGameObject;

public:
	std::string name;
	std::string fbxname;
	std::string pngname;
	uint index_name;
	uint normals_name;
	uint vertex_name;
	uint faces_name;
	uint texturescoords_name;
	GLuint actualtexgl;

	bool objSelected;
	bool ObjrenderActive;
	bool Objdelete;
	bool ObjtexActive;
	bool ObjnormActive;
	bool ObjdefauTex;
};