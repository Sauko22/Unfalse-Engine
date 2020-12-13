#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include <string>
#include <iostream>

//ImGui includes
#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

#include <vector>

class ModuleUI : public Module
{
public:
	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();

	bool Init();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

public:
	SDL_GLContext context;

	// Toolbar
	bool showDemo;
	bool showAbout;
	bool showConfig;
	bool showSceneLoad;

	// Docking
	bool showDock;

	// Primitives
	bool cube;
	bool pyramid;
	bool sphere;
	bool cylinder;

	void showAboutWin(bool* p_open = NULL);
	void showConfigWin(bool* p_open = NULL);
	void showSceneLoadWin(bool* p_open = NULL);
	void showConsoleWin();
	void showDockSpace(bool* p_open = NULL);
	void showInspectorWin();

	void Hierarchy(GameObject* gameobject);
	void DeselectGameObjects(GameObject* gameobject);
	void CreateEmptyGameObject();

	void ChangeGameObjectParent(GameObject* obj, GameObject* nextparent);
	void TimeWindows();

	void ResourceExplorer();
	void AssetsExplorer();
	
	bool playing;
	bool paused;
	bool bounding;
	bool raycast;
	
	// Editor menu
	bool activeInspec;
	bool activeConsole;
	bool activeHierach;

	void putLog(const char*);
	
	std::vector<std::string> items;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	vec3 cameradirection;
	bool wireframe;
	bool depth;
	bool cull;
	bool lighting;
	bool color;
	bool texture2d;
	bool vsync;
	int j, k, l;
	
	void DrawFolder();
	void DrawTree(const char* dire, const char* filter_extension);
	std::string folder;

	uint myscene;
	bool savescene;

	int my_image_width;
	int my_image_height;
	
	GLuint folde_;
	GLuint file_;
	GLuint texture_;
	GLuint fbx_;
	uint folders;
	uint file;
	uint fbx;
	uint texture;
};
