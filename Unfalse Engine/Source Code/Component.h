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
	virtual void inspector() {};

public:
	compType type;
	GameObject* gameObject;
	bool renderactive;
	bool normactive;
	bool texactive;
	bool deftexactive;
	GLuint newtexgl = 0;
	bool gameobject_selected;
	int texture_h;
	int texture_w;
	std::string texname;
};

class CompTransform : public Component
{
public:
	CompTransform(GameObject* gameobject);
	~CompTransform();

	void update();
	void inspector();
	void UpdateTrans();
	void SetPosition(float x, float y, float z);
	
public:
	float3 pos;
	float3 newpos;
	Quat rot;
	Quat newrot;
	float3 scl;
	float3 newscl;
	float4x4 local_transform;

	float4x4 global_transform;
	float4x4 cons_transform;
};

class CompMesh : public Component
{
public:
	CompMesh(GameObject* gameobject);
	~CompMesh();

	void update();
	void inspector();
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
	bool meshactive;
	bool texactive;
	bool deftexactive;
	std::string name;
	std::string deftexname;
};

class CompMaterial : public Component
{
public:
	CompMaterial(GameObject* gameobject);
	~CompMaterial();

	void update();
	void inspector();
	virtual void RenderTexture();

public:


};
