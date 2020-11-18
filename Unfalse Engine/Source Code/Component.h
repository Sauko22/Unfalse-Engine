#pragma once

#include "Globals.h"
#include "glmath.h"
#include <vector>
#include <string>

#include "MathGeoLib/include/MathGeoLib.h"

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
	GLuint newtexgl = 0;
};

class CompTransform : public Component
{
public:
	CompTransform(GameObject* gameobject);
	~CompTransform();

	void update();

public:
	float3 pos;
	Quat rot;
	float3 scl;
	float4x4 transform;
};

class CompMesh : public Component
{
public:
	CompMesh(GameObject* gameobject);
	~CompMesh();

	void update();
	void RenderMesh();

public:
	uint id_index; // index in VRAM
	uint num_index;
	uint* index = nullptr;

	uint id_vertex; // unique vertex in VRAM
	uint num_vertex;
	float* vertex;

	uint id_normals; // unique normals in VRAM
	uint num_normals;
	float* normals;

	uint id_tex; // unique tex coords in VRAM
	uint num_tex;
	float* tex;

	uint num_faces;

	GLuint textgl;
	GLuint defaultex;

	bool hastext;
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