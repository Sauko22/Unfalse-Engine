#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>
#include "Component.h"

typedef unsigned int GLuint;

struct EmptyGameObject
{
	virtual void CreateEmptyGameObject();
	//virtual void RenderEmptyGameObject() const;
	virtual void showEmptyInspectorWin();

	std::string name = " ";
	uint gameObjects = 0;

	std::vector<GameObject*> gameobject_list;

	bool emptySelected = false;
	bool emptyrenderActive = true;
};

class ModuleGameObject : public Module
{
public:
	ModuleGameObject(Application* app, bool start_enabled = true);
	~ModuleGameObject();

	bool Start();
	update_status Update();
	bool CleanUp();
	
public:
	bool active;

	// GameObject list
	std::vector<EmptyGameObject*> emptygameobject_list;

	std::vector<GameObject*> temp_gameobj_list;
};

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void showInspectorWin();

	void update();

	Component* AddComponent(Component::compType type);

	std::vector<Component*> component_list;
	std::vector<GameObject*> gameobject_list;
	//std::vector<GameObject*> gameobject_child_list;

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