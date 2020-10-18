#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleFBXLoad.h"
#include "glmath.h"
#include "Light.h"
#include "Vec2.h"

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

	void FitWinScene(Vec2 newSize);

	void GenerateSceneBuffers();

	void Draw();

	void Draw_Axis();
	void Draw_Mesh();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	// Draw axis
	float init[3] = { 0,0,0 };
	float X[3] = { 1,0,0 };
	float Y[3] = { 0,1,0 };
	float Z[3] = { 0,0,1 };

	// Render texture
	uint frameBuffer = 0;
	uint depthBuffer = 0;
	uint renderTexture = 0;

	Vec2 img_corner;
	Vec2 img_size;
	Vec2 cornerPos;
	Vec2 win_size;
	Vec2 img_offset;

	Mesh* mesh;
};