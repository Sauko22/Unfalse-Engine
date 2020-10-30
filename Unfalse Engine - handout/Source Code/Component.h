#pragma once

#include "Globals.h"
#include "glmath.h"
#include <vector>
#include <string>

//#include "MathGeoLib/include/MathGeoLib.h"

typedef unsigned int GLuint;
typedef unsigned int ILuint;

class GameObject;

class Component
{
public:
	enum class compType
	{
		NO_TYPE,
		TRANSFORM,
		MESH,
		MATERIAL
	};

	Component(compType type, GameObject*);
	virtual ~Component();

	virtual void update() {};

public:
	compType type;
	GameObject* gameObject;
	bool normactive;
	bool texactive;
	bool deftexactive;
};

class CompTransform : public Component
{
public:
	CompTransform(GameObject* gameobject, vec3 position, vec3 rotation, vec3 scale);
	~CompTransform();

	void update();

public:
	vec3 pos;
	vec3 rot;
	vec3 scl;
};

struct Mesh 
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

	uint id_normals = 0; // unique vertex in VRAM
	uint num_normals = 0;
	float* normals = nullptr;

	uint id_tex = 0; // unique vertex in VRAM
	uint num_tex = 0;
	float* tex = nullptr;

	GLuint textgl;
	GLuint defaultex;

	bool hastext = false;
};

class CompMesh : public Component
{
public:
	CompMesh(GameObject* gameobject);
	~CompMesh();

	void update();
	void RenderMesh();

public:
	Mesh* newmesh;
	std::vector<Mesh*> mesh_list;
};

class CompMaterial : public Component
{
public:
	CompMaterial(GameObject* gameobject);
	~CompMaterial();

	void update();
	virtual void RenderTexture();

public:

};
