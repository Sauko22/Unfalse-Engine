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
	bool showConsole;

	void showAboutWin(bool* p_open = NULL);
	void showConfigWin(bool* p_open = NULL);
	void showConsoleWin(bool* p_open = NULL);

	//void AddLogText(std::string incoming_text);

	// WINDOW EXAMPLES
	/*
	// Window 3 variables
	float f;
	char buf[20];

	// Window 4 variables
	float my_color[4];
	*/

	void putLog(const char*);
	void print_commits_info(const char* username, const char* repo);

	std::vector<std::string> items;

	std::vector<float> fps_log;
	std::vector<float> ms_log;

	bool wireframe;
	bool depth;
	bool cull;
	bool lighting;
	bool color;
	bool texture2d;
};
