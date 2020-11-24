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
	void UpdateAABB();
	const AABB& GetAABB() const;
	const OBB& GetOBB() const;

	Component* AddComponent(Component::compType type);
	Component* GetComponent(Component::compType type);

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

	OBB obb;
	AABB aabb;
};