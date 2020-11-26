#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Vec2.h"
#include "Component.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);

	void WinResize(Vec2 newSize);

	void GenerateSceneBuffers();

	void Draw();

	void Draw_Axis();

	// Update gameobjects
	void UpdateGameObjects(GameObject* gameobject);
	void DeleteGameObjects(GameObject* gameobject);
	void GenerateAABB(CompMesh* compmesh);
	void GenerateLines(CompMesh* compmesh);

	void MainCamera(CompCamera* camera);
	bool ContainsAaBox_2(AABB aabb);

public:

	Light lights[MAX_LIGHTS];
	//mat3x3 NormalMatrix;
	mat4x4 /*ModelMatrix, ViewMatrix,*/ ProjectionMatrix;

	// Draw axis
	float init[3] = { 0,0,0 };
	float X[3] = { 1,0,0 };
	float Y[3] = { 0,1,0 };
	float Z[3] = { 0,0,1 };

	// Show normal lines
	bool showlines;

	// Render texture
	uint frameBuffer;
	uint depthBuffer;
	uint renderTexture;
	Vec2 img_size;
	Vec2 win_size;

	int j;

	GLuint texchec;

	CompCamera* main_camera;
	bool culling;

};