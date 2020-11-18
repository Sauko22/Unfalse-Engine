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

	virtual void showInspectorWin();

	void update();

	Component* AddComponent(Component::compType type);

	GameObject* parentGameObject;

public:
	std::string name;
	std::string fbxname;
	std::string pngname;
	std::string deftexname;
	uint index_name;
	uint normals_name;
	uint vertex_name;
	uint faces_name;
	uint texturescoords_name;
	GLuint actualtexgl;

	int texture_h;
	int texture_w;

	bool objSelected;
	bool ObjrenderActive;
	bool ObjtexActive;
	bool ObjnormActive;
	bool ObjdefauTex;
};