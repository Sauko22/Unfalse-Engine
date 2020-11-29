#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Component.h"
#include "GameObject.h"

#include "MathGeoLib/include/MathGeoLib.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrixCamera();


public:
	
	float X, Y, Z;
	float3 Position, Reference;
	float camera_speed;
	bool lalt;
	bool orbit;
	CompCamera* scene_camera;
	CompCamera* _scene_camera;
	CompTransform* scene_transform;
	CompTransform* _scene_transform;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	
	float3 origin;
	float3 dest;
	float offsetx;
};