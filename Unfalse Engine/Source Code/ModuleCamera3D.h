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

	void Look(const vec3 &Spot);
	float* GetViewMatrixCamera();
	void ObjPicked(LineSegment ray_local_space);
	
	float4x4 GetViewMatrixM();
	
	float4x4 GetProjectionMatrixM();

public:
	
	vec3 X, Y, Z;
	vec3 Reference, Position, _Position;
	float camera_speed;
	bool lalt;
	bool orbit;
	CompCamera* scene_camera;
	CompCamera* _scene_camera;
	CompTransform* scene_transform;
	CompTransform* _scene_transform;

private:

	float4x4 RotMatrix;
	
	float3 origin;
	float3 dest;
	float offsetx;
	float offsety;
	bool first_it;
};