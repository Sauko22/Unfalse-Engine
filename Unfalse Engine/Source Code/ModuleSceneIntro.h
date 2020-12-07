#pragma once
#include "Module.h"

#include "Globals.h"
#include "Timer.h"
#include "PathNode.h"
#include <vector>
#include "GameObject.h"


#define BOUNCER_TIME 200
#define FPS_INTERVAL 1.0 //seconds.

#include "Imgui/imgui.h"
#include "Imgui/imgui_internal.h"
#include "Imgui/imgui_impl_sdl.h"
#include "Imgui/imgui_impl_opengl3.h"
#include "ImGui/ImGuizmo.h"
struct PhysBody3D;
class Cube;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

	void GetAllGameObjects();
	void AllGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects);
	void HandleInput();
	void EditTransform();
public:
	// Root gameobject
	GameObject* root;
	GameObject* SelectedGameObject;
	GameObject* camera;
	std::vector<GameObject*> gameobject_list;

	ImGuizmo::OPERATION mCurrentGizmoOperation;
	ImGuizmo::MODE mCurrentGizmoMode;


	Uint32 fps_lasttime; //the last recorded time.
	Uint32 fps_current; //the current FPS.
	Uint32 fps_frames; //frames passed since the last recorded fps
	Vec2 cornerPos;
	//The frames per second
	int FRAMES_PER_SECOND;
	
	//Keep track of the current frame
	int frame;

	//The frame rate regulator
	Timer fps;
};
